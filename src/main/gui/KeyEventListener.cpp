#include "KeyEventListener.h"
#include "Constants.h"

#include <Mpc.hpp>

#include <controls/Controls.hpp>
#include <controls/KeyEvent.hpp>
#include <controls/KeyEventHandler.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/DataWheel.hpp>
#include <hardware/Led.hpp>

#include <cmrc/cmrc.hpp>
#include <string_view>

#include "../version.h"

CMRC_DECLARE(vmpcjuce);

juce::Image loadImage2(std::string path)
{
  auto fs = cmrc::vmpcjuce::get_filesystem();
  auto file = fs.open(path.c_str());
  auto data = std::string_view(file.begin(), file.size()).data();
  auto stream = juce::MemoryInputStream(data, file.size(), true);
  return juce::ImageFileFormat::loadFrom(stream);
}

KeyEventListener::KeyEventListener(mpc::Mpc& _mpc)
: mpc (_mpc), keyEventHandler (mpc.getControls().lock()->getKeyEventHandler())
{
  setName("KeyEventListener");
  dataWheel = new DataWheelControl(mpc.getHardware().lock()->getDataWheel());
  mpc.getHardware().lock()->getDataWheel().lock()->addObserver(dataWheel);
  dataWheelImg = loadImage2("img/datawheels.jpg");
  dataWheel->setImage(dataWheelImg, 100);
  addAndMakeVisible(dataWheel);
  
  lcd = new LCDControl(mpc, mpc.getLayeredScreen());
  lcd->setSize(496, 120);
  addAndMakeVisible(lcd);
  
  lcd->drawPixelsToImg();
  lcd->startTimer(25);
  
  ButtonControl::initRects();
  
  for (auto& l : mpc.getHardware().lock()->getButtonLabels())
  {
    auto bc = new ButtonControl(ButtonControl::rects[l]->expanded(10),
                                mpc.getHardware().lock()->getButton(l));
    addAndMakeVisible(bc);
    buttons.push_back(bc);
  }
  
  const int padWidth = 96;
  int padSpacing = 25;
  const int padOffsetX = 778;
  const int padOffsetY = 397;
  int padCounter = 0;
  
  padHitImg = loadImage2("img/padhit.png");
  
  for (int j = 3; j >= 0; j--)
  {
    for (int i = 0; i < 4; i++)
    {
      int x1 = (padWidth + padSpacing) * i + padOffsetX + i;
      int y1 = (padWidth + padSpacing) * j + padOffsetY;
      Rectangle<float> rect(x1, y1, padWidth + i, padWidth);
      
      auto pc = new PadControl(mpc, rect, mpc.getHardware().lock()->getPad(padCounter++), padHitImg);
      addAndMakeVisible(pc);
      
      pads.push_back(pc);
    }
  }
  
  sliderImg = loadImage2("img/sliders.jpg");
  slider = new SliderControl(mpc.getHardware().lock()->getSlider());
  slider->setImage(sliderImg);
  addAndMakeVisible(slider);
  
  recKnobImg = loadImage2("img/recknobs.jpg");
  recKnob = new KnobControl(0, mpc.getHardware().lock()->getRecPot());
  recKnob->setImage(recKnobImg);
  addAndMakeVisible(recKnob);
  
  volKnobImg = loadImage2("img/volknobs.jpg");
  volKnob = new KnobControl(0, mpc.getHardware().lock()->getVolPot());
  volKnob->setImage(volKnobImg);
  addAndMakeVisible(volKnob);
  
  ledRedImg = loadImage2("img/led_red.png");
  ledGreenImg = loadImage2("img/led_green.png");
  
  leds = new LedControl(mpc, ledGreenImg, ledRedImg);
  leds->setPadBankA(true);
  leds->addAndMakeVisible(this);
  
  for (auto& l : mpc.getHardware().lock()->getLeds())
    l->addObserver(leds);
  
  for (auto& l : mpc.getHardware().lock()->getLeds())
    l->deleteObserver(leds);
  
  leds->startTimer(25);
  slider->startTimer(25);
  
}

KeyEventListener::~KeyEventListener()
{
  delete dataWheel;
  
  lcd->stopTimer();
  delete lcd;
  
  for (auto& b : buttons)
    delete b;
  
  for (auto& p : pads)
    delete p;
  
  delete leds;
  delete recKnob;
  delete volKnob;
  delete slider;
}

bool KeyEventListener::keyPressed(const juce::KeyPress& k)
{
  if (k.getTextDescription().toStdString() == "command + Q")
    return false;
  
  return true;
}

bool KeyEventListener::keyEvent(const juce::KeyEvent &keyEvent)
{
  keyEventHandler.lock()->handle(mpc::controls::KeyEvent(keyEvent.rawKeyCode, keyEvent.keyDown));
  return true;
}

void KeyEventListener::mouseDown(const juce::MouseEvent& e) {
  bool exists = false;
  for (auto& s : sources) {
    if (s->getIndex() == e.source.getIndex()) { exists = true; break; }
  }
  if (!exists) sources.push_back(std::make_shared<juce::MouseInputSource>(e.source));
  
  auto pos1 = sources[0]->getLastMouseDownPosition();
  
  if (sources.size() == 1) {
    prevSingleX = pos1.getX();
    prevSingleY = pos1.getY();
  } else if (sources.size() == 2) {
    auto pos2 = sources[1]->getLastMouseDownPosition();
    float length = juce::Line<int>(pos1.getX(), pos1.getY(), pos2.getX(), pos2.getY()).getLength();
    prevDistance = length;
  }
}

void KeyEventListener::mouseUp(const juce::MouseEvent& e)
{
  for (int i = 0; i < sources.size(); i++) {
    if (sources[i]->getIndex() == e.source.getIndex()) {
      sources.erase(sources.begin() + i);
      prevDistance = -1.f;
      prevPinchCx = -1.f;
      prevPinchCy = -1.f;
      break;
    }
  }
}

void KeyEventListener::mouseDrag(const juce::MouseEvent& e) {
  
  auto thisSource = e.source;
  auto cur_pos1 = thisSource.getScreenPosition();
  auto content = getParentComponent();
  
  if (sources.size() == 1) {
    auto translation_x = prevSingleX != -1.f ? (cur_pos1.getX() - prevSingleX) : 0.f;
    auto translation_y = prevSingleY != -1.f ? (cur_pos1.getY() - prevSingleY) : 0.f;
    prevSingleX = cur_pos1.getX();
    prevSingleY = cur_pos1.getY();
    auto newX = content->getX() + translation_x;
    auto newY = content->getY() + translation_y;
    content->setBounds(newX, newY, content->getWidth(), content->getHeight());
  }
  else if (sources.size() == 2) {
    
    // For smooth transitio between 1 and 2 fingers:
    // We should keep track of prevxy in a map per e.source.getIndex
    prevSingleX = -1.f;
    prevSingleY = -1.f;
    
    std::shared_ptr<juce::MouseInputSource> thatSource;
    
    for (auto& s : sources) {
      if (s->getIndex() != thisSource.getIndex()) {
        thatSource = s;
        break;
      }
    }
    
    auto cur_pos2 = thatSource->getScreenPosition();
    
    float cur_distance = juce::Line<int>(cur_pos1.getX(), cur_pos1.getY(), cur_pos2.getX(), cur_pos2.getY()).getLength();
    
    auto scale = prevDistance != -1 ? (cur_distance / prevDistance) : 1.0f;
    prevDistance = cur_distance;
    auto new_w = content->getWidth() * scale;
    auto ratio = 1298.0 / 994.0;
    auto new_h = new_w / ratio;
    
    auto contentScale = content->getWidth() / 1298.0;
    auto screen_pinch_cx = (cur_pos1.getX() + cur_pos2.getX()) / 2;
    auto current_x = content->getX();
    auto current_content_pinch_cx = (screen_pinch_cx * contentScale) - current_x;
    auto new_content_pinch_cx = current_content_pinch_cx * scale;
    auto new_x = (screen_pinch_cx * contentScale) - new_content_pinch_cx;
    
    auto screen_pinch_cy = (cur_pos1.getY() + cur_pos2.getY()) / 2;
    auto current_y = content->getY();
    auto current_content_pinch_cy = (screen_pinch_cy * contentScale) - current_y;
    auto new_content_pinch_cy = current_content_pinch_cy * scale;
    auto new_y = (screen_pinch_cy * contentScale) - new_content_pinch_cy;
    
    auto translation_x = prevPinchCx != -1.f ? (screen_pinch_cx - prevPinchCx) : 0.f;
    auto translation_y = prevPinchCy != -1.f ? (screen_pinch_cy - prevPinchCy) : 0.f;
    
    translation_x *= 1.2f;
    translation_y *= 1.2f;
    
    new_x += translation_x;
    new_y += translation_y;
    
    prevPinchCx = screen_pinch_cx;
    prevPinchCy = screen_pinch_cy;
    
    auto primaryDisplay = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay();
    
    if (primaryDisplay != nullptr) {
      auto area = primaryDisplay->userArea;
      auto portrait = area.getWidth() < area.getHeight();
      
      if (portrait && new_h < area.getHeight()) {
        return;
      }
      else if (!portrait && new_w < area.getWidth()) {
        return;
      }
      else if (portrait && new_h > area.getHeight() * 3.0) {
        return;
      }
      else if (!portrait && new_w > area.getWidth() * 3.0) {
        return;
      }
      
      content->setBounds(new_x, new_y, new_w, new_h);
    }
  }
  
  keyboardImg = loadImage2("img/keyboard.png");
  
  keyboardButton.setImages(false, true, true, keyboardImg, 0.5, Colours::transparentWhite, keyboardImg, 1.0, Colours::transparentWhite, keyboardImg, 0.25, Colours::transparentWhite);
  
  resetWindowSizeImg = loadImage2("img/reset-window-size.png");
  
  resetWindowSizeButton.setImages(false, true, true, resetWindowSizeImg, 0.5, Colours::transparentWhite, resetWindowSizeImg, 1.0, Colours::transparentWhite, resetWindowSizeImg, 0.25, Colours::transparentWhite);
  
  versionLabel.setText(version::get(), dontSendNotification);
  versionLabel.setColour(Label::textColourId, Colours::darkgrey);
  addAndMakeVisible(versionLabel);
  
  keyboardButton.setTooltip("Configure computer keyboard");
  
  class KbButtonListener : public Button::Listener {
  public:
    KbButtonListener(mpc::Mpc& _mpc) : mpc(_mpc) {}
    mpc::Mpc& mpc;
    void buttonClicked(Button*) override {
      mpc.getLayeredScreen().lock()->openScreen("vmpc-keyboard");
    }
  };
  
  keyboardButton.addListener(new KbButtonListener(mpc));
  keyboardButton.setWantsKeyboardFocus(false);
  addAndMakeVisible(keyboardButton);
  
  resetWindowSizeButton.setTooltip("Reset window size");
  
  class ResetButtonListener : public Button::Listener {
  public:
    ResetButtonListener(mpc::Mpc& _mpc, Component* __this) : mpc(_mpc), _this(__this) {}
    mpc::Mpc& mpc;
    Component* _this;
    void buttonClicked(Button*) override {
      _this->getParentComponent()->setSize(1298 / 2, 994 /2);
    }
  };
  
  resetWindowSizeButton.addListener(new ResetButtonListener(mpc, this));
  resetWindowSizeButton.setWantsKeyboardFocus(false);
  //  addAndMakeVisible(resetWindowSizeButton);
}

void KeyEventListener::resized()
{
  auto scale = static_cast<float>(getWidth() / 1298.0);
  auto scaleTransform = juce::AffineTransform::scale(scale);
  dataWheel->setTransform(scaleTransform);
  dataWheel->setBounds(Constants::DATAWHEEL_RECT()->getX(), Constants::DATAWHEEL_RECT()->getY(), dataWheel->getFrameWidth(), dataWheel->getFrameHeight());
  lcd->setTransform(scaleTransform);
  lcd->setBounds(Constants::LCD_RECT()->getX(), Constants::LCD_RECT()->getY(), 496, 120);
  
  for (auto& b : buttons)
  {
    b->setTransform(scaleTransform);
    b->setBounds();
  }
  
  for (auto& p : pads)
  {
    p->setBounds();
    p->setTransform(scaleTransform);
  }
  
  leds->setTransform(scaleTransform);
  leds->setBounds();
  
  slider->setTransform(scaleTransform);
  slider->setBounds(Constants::SLIDER_RECT()->getX(), Constants::SLIDER_RECT()->getY(), sliderImg.getWidth() / 2, sliderImg.getHeight() * 0.01 * 0.5);
  
  recKnob->setTransform(scaleTransform);
  recKnob->setBounds(Constants::RECKNOB_RECT()->getX(), Constants::RECKNOB_RECT()->getY(), recKnobImg.getWidth() / 2, recKnobImg.getHeight() * 0.01 * 0.5);
  
  volKnob->setTransform(scaleTransform);
  volKnob->setBounds(Constants::VOLKNOB_RECT()->getX(), Constants::VOLKNOB_RECT()->getY(), volKnobImg.getWidth() / 2, volKnobImg.getHeight() * 0.01 * 0.5);
  
  keyboardButton.setBounds(1298 - (100 +  10), 10, 100, 50);
  keyboardButton.setTransform(scaleTransform);
  //
  //  resetWindowSizeButton.setBounds(1298 - (145 + 20), 13, 45, 45);
  //  resetWindowSizeButton.setTransform(scaleTransform);
  
  versionLabel.setTransform(scaleTransform);
  versionLabel.setBounds(1175, 118, 100, 20);
}
