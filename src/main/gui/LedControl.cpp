#include "LedControl.hpp"

#include <Mpc.hpp>

#include <controls/Controls.hpp>

#include <sequencer/Sequencer.hpp>

#include <string>

LedControl::LedControl(mpc::Mpc& _mpc, juce::Image& _ledGreen, juce::Image& _ledRed)
: mpc (_mpc), ledGreen (_ledGreen), ledRed (_ledRed)
{
	int x, y;
	int ledSize = 10;

	x = 874;
	y = 216;

	sixteenLevels = juce::Rectangle<float>(x, y, ledSize, ledSize);
	x = 791;
	y = 298;
	nextSeq = juce::Rectangle<float>(x, y, ledSize, ledSize);
	x = 875;
	y = 298;
	trackMute = juce::Rectangle<float>(x, y, ledSize, ledSize);
	x = 103;
	y = 601;
	after = juce::Rectangle<float>(x, y, ledSize, ledSize);
	x = 226;
	y = 686;
	undoSeq = juce::Rectangle<float>(x, y, ledSize, ledSize);
	x = 214;
	y = 833;
	rec = juce::Rectangle<float>(x, y, ledSize, ledSize);
	x = 294;
	y = 833;
	overDub = juce::Rectangle<float>(x, y, ledSize, ledSize);
	x = 451;
	y = 830;
	play = juce::Rectangle<float>(x, y, ledSize, ledSize);
	x = 791;
	y = 216;
	fullLevel = juce::Rectangle<float>(x, y, ledSize, ledSize);
	x = 958;
	y = 298;
	padBankA = juce::Rectangle<float>(x, y, ledSize, ledSize);
	x = 1041;
	y = 298;
	padBankB = juce::Rectangle<float>(x, y, ledSize, ledSize);
	x = 1124;
	y = 297;
	padBankC = juce::Rectangle<float>(x, y, ledSize, ledSize);
	x = 1206;
	y = 296;
	padBankD = juce::Rectangle<float>(x, y, ledSize, ledSize);

	fullLevelLed = new Led(ledGreen, fullLevel);
	sixteenLevelsLed = new Led(ledGreen, sixteenLevels);
	nextSeqLed = new Led(ledGreen, nextSeq);
	trackMuteLed = new Led(ledGreen, trackMute);
	padBankALed = new Led(ledGreen, padBankA);
	padBankBLed = new Led(ledGreen, padBankB);
	padBankCLed = new Led(ledGreen, padBankC);
	padBankDLed = new Led(ledGreen, padBankD);
	afterLed = new Led(ledGreen, after);
	undoSeqLed = new Led(ledRed, undoSeq);
	recLed = new Led(ledRed, rec);
	overDubLed = new Led(ledRed, overDub);
	playLed = new Led(ledGreen, play);
}

void LedControl::addAndMakeVisible(juce::Component* parent) {
	parent->addAndMakeVisible(padBankALed);
	parent->addAndMakeVisible(fullLevelLed);
	parent->addAndMakeVisible(sixteenLevelsLed);
	parent->addAndMakeVisible(nextSeqLed);
	parent->addAndMakeVisible(trackMuteLed);
	parent->addAndMakeVisible(padBankALed);
	parent->addAndMakeVisible(padBankBLed);
	parent->addAndMakeVisible(padBankCLed);
	parent->addAndMakeVisible(padBankDLed);
	parent->addAndMakeVisible(afterLed);
	parent->addAndMakeVisible(undoSeqLed);
	parent->addAndMakeVisible(recLed);
	parent->addAndMakeVisible(overDubLed);
	parent->addAndMakeVisible(playLed);
}

void LedControl::setTransform(juce::AffineTransform transform) {
	fullLevelLed->setTransform(transform);
	sixteenLevelsLed->setTransform(transform);
	nextSeqLed->setTransform(transform);
	trackMuteLed->setTransform(transform);
	padBankALed->setTransform(transform);
	padBankBLed->setTransform(transform);
	padBankCLed->setTransform(transform);
	padBankDLed->setTransform(transform);
	afterLed->setTransform(transform);
	undoSeqLed->setTransform(transform);
	recLed->setTransform(transform);
	overDubLed->setTransform(transform);
	playLed->setTransform(transform);
}

void LedControl::setBounds() {
	fullLevelLed->setBounds();
	sixteenLevelsLed->setBounds();
	nextSeqLed->setBounds();
	trackMuteLed->setBounds();
	padBankALed->setBounds();
	padBankBLed->setBounds();
	padBankCLed->setBounds();
	padBankDLed->setBounds();
	afterLed->setBounds();
	undoSeqLed->setBounds();
	recLed->setBounds();
	overDubLed->setBounds();
	playLed->setBounds();
}


void LedControl::setPadBankA(bool b)
{
    padBankALed->setOn(b);
}

void LedControl::setPadBankB(bool b)
{
    padBankBLed->setOn(b);
}

void LedControl::setPadBankC(bool b)
{
    padBankCLed->setOn(b);
}

void LedControl::setPadBankD(bool b)
{
    padBankDLed->setOn(b);
}

void LedControl::setFullLevel(bool b)
{
    fullLevelLed->setOn(b);
}

void LedControl::setSixteenLevels(bool b)
{
    sixteenLevelsLed->setOn(b);
}

void LedControl::setNextSeq(bool b)
{
    nextSeqLed->setOn(b);
}

void LedControl::setTrackMute(bool b)
{
    trackMuteLed->setOn(b);
}

void LedControl::setAfter(bool b)
{
    afterLed->setOn(b);
}

void LedControl::setRec(bool b)
{
    recLed->setOn(b);
}

void LedControl::setOverDub(bool b)
{
    overDubLed->setOn(b);
}

void LedControl::setPlay(bool b)
{
    playLed->setOn(b);
}

void LedControl::setUndoSeq(bool b)
{
    undoSeqLed->setOn(b);
}

void LedControl::timerCallback()
{
    auto seq = mpc.getSequencer();
    auto controls = mpc.getControls();
    auto stepEditor = mpc.getLayeredScreen()->getCurrentScreenName() == "step-editor";

    setUndoSeq(seq->isUndoSeqAvailable());

    setPlay(seq->isPlaying());

    auto isPlayingButNotOverdubbingAndOverdubIsPressed = seq->isPlaying() && !seq->isOverDubbing() && controls->isOverDubPressed();

    if (isPlayingButNotOverdubbingAndOverdubIsPressed)
    {
        setOverDub(false);
    }
    else
    {
        setOverDub(controls->isOverDubPressed() || seq->isOverDubbing() || stepEditor);
    }

    auto isPlayingButNotRecordingAndRecIsPressed = seq->isPlaying() && !seq->isRecording() && controls->isRecPressed();

    if (isPlayingButNotRecordingAndRecIsPressed)
    {
        setRec(false);
    } else {
        setRec(controls->isRecPressed() || seq->isRecording());
    }
}

void LedControl::update(moduru::observer::Observable*, nonstd::any arg)
{
	auto s = nonstd::any_cast<std::string>(arg);

	if (s == "full-level-on") {
		setFullLevel(true);
	}
	else if(s == "full-level-off") {
		setFullLevel(false);
	}
	else if (s == "sixteen-levels-on") {
		setSixteenLevels(true);
	}
	else if (s == "sixteen-levels-off") {
		setSixteenLevels(false);
	}
	else if (s == "next-seq-on") {
		setNextSeq(true);
	}
	else if (s == "next-seq-off") {
		setNextSeq(false);
	}
	else if (s == "track-mute-on") {
		setTrackMute(true);
	}
	else if (s == "track-mute-off") {
		setTrackMute(false);
	}
	else if (s == "pad-bank-a-on") {
		setPadBankA(true);
	}
	else if (s == "pad-bank-a-off") {
		setPadBankA(false);
	}
	else if (s == "pad-bank-b-on") {
		setPadBankB(true);
	}
	else if (s == "pad-bank-b-off") {
		setPadBankB(false);
	}
	else if (s == "pad-bank-c-on") {
		setPadBankC(true);
	}
	else if (s == "pad-bank-c-off") {
		setPadBankC(false);
	}
	else if (s == "pad-bank-d-on") {
		setPadBankD(true);
	}
	else if (s == "pad-bank-d-off") {
		setPadBankD(false);
	}
	else if (s == "after-on") {
		setAfter(true);
	}
	else if (s == "after-off") {
		setAfter(false);
	}
	else if (s == "undo-seq-on") {
		setUndoSeq(true);
	}
	else if (s == "undo-seq-off") {
		setUndoSeq(false);
	}
	else if (s == "rec-on") {
		setRec(true);
	}
	else if (s == "rec-off") {
		setRec(false);
	}
	else if (s == "overdub-on") {
		setOverDub(true);
	}
	else if (s == "overdub-off") {
		setOverDub(false);
	}
}

LedControl::~LedControl() {
	delete fullLevelLed;
	delete sixteenLevelsLed;
	delete nextSeqLed;
	delete trackMuteLed;
	delete padBankALed;
	delete padBankBLed;
	delete padBankCLed;
	delete padBankDLed;
	delete afterLed;
	delete undoSeqLed;
	delete recLed;
	delete overDubLed;
	delete playLed;
}
