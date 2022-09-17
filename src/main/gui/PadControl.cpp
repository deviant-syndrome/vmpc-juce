#include "PadControl.hpp"
#include <hardware/HwPad.hpp>

#include <Mpc.hpp>

#include <sequencer/Track.hpp>

#include <sampler/Pad.hpp>
#include <sampler/NoteParameters.hpp>

#include <disk/SoundLoader.hpp>
#include <disk/MpcFile.hpp>

#include <lcdgui/screens/window/VmpcConvertAndLoadWavScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lang/StrUtil.hpp>

#include <Logger.hpp>

#include <math.h>

using namespace juce;
using namespace mpc::disk;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace moduru::lang;

PadControl::PadControl(mpc::Mpc &_mpc, juce::Rectangle<float> _rect, std::weak_ptr<mpc::hardware::HwPad> _pad,
                       Image _padHitImg)
        : VmpcTooltipComponent(_mpc, _pad.lock()), mpc(_mpc), pad(_pad), padhitImg(_padHitImg), rect(_rect)
{
    pad.lock()->addObserver(this);
}

bool PadControl::isInterestedInFileDrag(const StringArray &files)
{
    if (files.size() != 1)
    {
        return false;
    }

    for (auto &s: files)
    {
        if (StrUtil::hasEnding(StrUtil::toLower(s.toStdString()), ".snd") ||
            StrUtil::hasEnding(StrUtil::toLower(s.toStdString()), ".wav"))
        {
            if (padhitBrightness == 0)
            {
                fading = true;
                padhitBrightness = 152;
                repaint();
                startTimer(100);
            }
            return true;
        }
    }
    return false;
}

void PadControl::loadFile(const String path, bool shouldBeConverted, std::string screenToReturnTo)
{
    if (StrUtil::hasEnding(StrUtil::toLower(path.toStdString()), ".snd") ||
        StrUtil::hasEnding(StrUtil::toLower(path.toStdString()), ".wav"))
    {
        auto sampler = mpc.getSampler().lock();

        auto soundLoader = SoundLoader(mpc, sampler->getSounds(), false);
        soundLoader.setPreview(false);

        auto compatiblePath = StrUtil::replaceAll(path.toStdString(), '\\', std::string("\\"));

        auto file = std::make_shared<mpc::disk::MpcFile>(fs::path(compatiblePath));

        auto layeredScreen = mpc.getLayeredScreen().lock();

        SoundLoaderResult result;

        try
        {
            soundLoader.loadSound(file, result, shouldBeConverted);
        }
        catch (const std::exception &exception)
        {
            MLOG("A problem occurred when trying to load " + compatiblePath + ": " + std::string(exception.what()));
            MLOG(result.errorMessage);
            layeredScreen->openScreen(layeredScreen->getPreviousScreenName());
            return;
        }

        auto popupScreen = mpc.screens->get<PopupScreen>("popup");

        if (!result.success)
        {
            if (result.soundWasAdded) sampler->deleteSound(sampler->getPreviewSound());

            if (result.canBeConverted)
            {
                auto loadRoutine = [&, path, screenToReturnTo, layeredScreen]() {
                    const bool shouldBeConverted2 = true;
                    loadFile(path, shouldBeConverted2, screenToReturnTo);
                };

                auto convertAndLoadWavScreen = mpc.screens->get<VmpcConvertAndLoadWavScreen>(
                        "vmpc-convert-and-load-wav");
                convertAndLoadWavScreen->setLoadRoutine(loadRoutine);
                layeredScreen->openScreen("vmpc-convert-and-load-wav");
            }
            else
            {
                layeredScreen->openScreen("popup");
                popupScreen->setText(result.errorMessage);
                popupScreen->returnToScreenAfterMilliSeconds("load", 500);
            }

            return;
        }

        if (result.existingIndex == -1)
        {
            auto soundFileName = StrUtil::toUpper(file->getNameWithoutExtension());
            if (soundFileName.length() >= 16) soundFileName = soundFileName.substr(0, 16);

            auto ext = file->getExtension();

            popupScreen->setText("LOADING " + StrUtil::padRight(soundFileName, " ", 16) + "." + ext);

            layeredScreen->openScreen("popup");
            popupScreen->returnToScreenAfterMilliSeconds(screenToReturnTo, 300);

            auto drumIndex = mpc.getSequencer().lock()->getActiveTrack().lock()->getBus() - 1;

            if (drumIndex == -1)
            {
                layeredScreen->openScreen(screenToReturnTo);
                return;
            }

            auto mpcSoundPlayerChannel = mpc.getDrum(drumIndex);

            auto programIndex = mpcSoundPlayerChannel->getProgram();
            auto program = mpc.getSampler().lock()->getProgram(programIndex).lock();
            auto soundIndex = mpc.getSampler().lock()->getSoundCount() - 1;
            auto padIndex = pad.lock()->getIndex() + (mpc.getBank() * 16);
            auto programPad = program->getPad(padIndex);
            auto padNote = programPad->getNote();

            auto noteParameters = dynamic_cast<mpc::sampler::NoteParameters *>(program->getNoteParameters(padNote));

            if (noteParameters == nullptr)
            {
                layeredScreen->openScreen(layeredScreen->getPreviousScreenName());
                return;
            }

            noteParameters->setSoundIndex(soundIndex);
        }
    }
}

void PadControl::filesDropped(const StringArray &files, int, int)
{
    if (files.size() != 1) return;

    const bool shouldBeConverted = false;
    std::string screenToReturnTo = mpc.getLayeredScreen().lock()->getCurrentScreenName();

    for (auto &f: files)
    {
        loadFile(f, shouldBeConverted, screenToReturnTo);
    }
}

void PadControl::timerCallback()
{
    if (fading)
    {
        padhitBrightness -= 20;
    }

    if (padhitBrightness < 0)
    {
        padhitBrightness = 0;
        repaint();
        fading = false;
        stopTimer();
    }
    else
    {
        repaint();
    }
}

void PadControl::update(moduru::observer::Observable *, nonstd::any arg)
{
    int velocity = nonstd::any_cast<int>(arg);

    if (velocity == 255)
    {
        fading = true;
        pressed = false;
    }
    else
    {
        padhitBrightness = velocity + 25;
        pressed = true;
        fading = false;
        startTimer(100);
    }
}

int PadControl::getVelo(int x, int y)
{
    float centX = rect.getCentreX() - rect.getX();
    float centY = rect.getCentreY() - rect.getY();
    float distX = x - centX;
    float distY = y - centY;
    float powX = static_cast<float>(pow(distX, 2));
    float powY = static_cast<float>(pow(distY, 2));
    float dist = sqrt(powX + powY);
    if (dist > 46) dist = 46;
    int velo = static_cast<int>(127.0 - (dist * (127.0 / 48.0)));
    return velo;
}

void PadControl::mouseDown(const MouseEvent &event)
{
    pad.lock()->push(getVelo(event.x, event.y));
}

void PadControl::mouseDoubleClick(const MouseEvent &)
{
}

void PadControl::mouseUp(const MouseEvent &)
{
    pad.lock()->release();
}

void PadControl::mouseDrag(const MouseEvent &event)
{
    auto controls = mpc.getControls().lock();
    auto padIndex = pad.lock()->getIndex();
    if (controls->getPressedPads()->find(padIndex) == controls->getPressedPads()->end())
        return;

    auto newVelo = getVelo(event.x, event.y);

    (*controls->getPressedPadVelos())[padIndex] = newVelo;
}

void PadControl::setBounds()
{
    setSize(rect.getWidth(), rect.getHeight());
    Component::setBounds(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

void PadControl::paint(Graphics &g)
{
    auto img = padhitImg.createCopy();
    float mult = (float) (padhitBrightness) / 150.0;
    img.multiplyAllAlphas(mult);
    g.drawImageAt(img, 0, 0);
}

PadControl::~PadControl()
{
    pad.lock()->deleteObserver(this);
}
