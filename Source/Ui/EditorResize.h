/*
  ==============================================================================

    EditorResize.h
    Created: 13 Apr 2025 3:57:46pm
    Author:  Myles Wang

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class EditorResize final: public juce::LookAndFeel_V4
{
    public:
    EditorResize();
    
    void drawCornerResizer (juce::Graphics& g, int w, int h, bool isMouseOver, bool isMouseDragging) override;
    
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditorResize)
};
