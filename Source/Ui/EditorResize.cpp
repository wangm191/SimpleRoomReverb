/*
  ==============================================================================

    EditorResize.cpp
    Created: 13 Apr 2025 3:57:55pm
    Author:  Myles Wang

  ==============================================================================
*/

#include "EditorResize.h"
#include "UseColors.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

EditorResize :: EditorResize() {

}

void EditorResize :: drawCornerResizer(juce::Graphics& g, int w, int h, bool isMouseOver, bool isMouseDragging)
{
    juce::ignoreUnused (isMouseDragging);
    
    const auto width = static_cast<float>(w);
    const auto height = static_cast<float>(h);
    const auto lineThickness = juce::jmin(width, height) * 0.07f;
    
    g.setColour(isMouseOver ? UseColors::red : UseColors::green);
    
    for (float i = 0.0f; i < 1.0f; i += 0.33f)
    {
        g.drawLine(width * i, height + 1.0f, width + 1.0f, height * i, lineThickness);
        g.drawLine(width * i + lineThickness, height + 1.0f, width + 1.0f, height * i + lineThickness, lineThickness);
    }
}
