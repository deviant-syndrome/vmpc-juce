#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct Background : public juce::Component {

public:
  Background();
  
  void paint(juce::Graphics& g) override;
  
  void mouseDown(const juce::MouseEvent& e) override {
    getParentComponent()->mouseDown(e);
  }
  
  void mouseUp(const juce::MouseEvent& e) override {
    getParentComponent()->mouseUp(e);
  }
  
  void mouseDrag(const juce::MouseEvent& e) override {
    getParentComponent()->mouseDrag(e);
  }
  
private:
  juce::Image img;

};
