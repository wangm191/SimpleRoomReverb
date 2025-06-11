/*
  ==============================================================================

    EditorContent.cpp
    Created: 12 Apr 2025 4:39:43pm
    Author:  Myles Wang

  ==============================================================================
*/

#include "EditorContent.h"
#include "../Parameters.h"

EditorContent::EditorContent (SimpleRoomReverbAudioProcessor& p, juce::UndoManager& um)
    : apvts( p.getPluginState())
    , sizeSlider(*apvts.getParameter(Parameters::size), &um)
    , dampSlider(*apvts.getParameter(Parameters::damp), &um)
    , widthSlider(*apvts.getParameter(Parameters::width), &um)
    , mixSlider(*apvts.getParameter(Parameters::mix), &um)
    , lowPassSlider(*apvts.getParameter(Parameters::lowPass), &um)
    , highPassSlider(*apvts.getParameter(Parameters::highPass), &um)
    , freezeButton(*apvts.getParameter(Parameters::freeze), &um)
    , bypassButton(*apvts.getParameter(Parameters::bypass), &um)
    , undoButton(um, UndoManagerButton::ActionType::Undo)
    , redoButton(um, UndoManagerButton::ActionType::Redo)
{
    setWantsKeyboardFocus (true);
    setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    
    sizeSlider.setExplicitFocusOrder(1);
    dampSlider.setExplicitFocusOrder(2);
    widthSlider.setExplicitFocusOrder(3);
    mixSlider.setExplicitFocusOrder(4);
    lowPassSlider.setExplicitFocusOrder(5);
    highPassSlider.setExplicitFocusOrder(6);
    
    addAndMakeVisible (sizeSlider);
    addAndMakeVisible (dampSlider);
    addAndMakeVisible (widthSlider);
    addAndMakeVisible (mixSlider);
    addAndMakeVisible (lowPassSlider);
    addAndMakeVisible (highPassSlider);
    
    addAndMakeVisible(freezeButton);
    addAndMakeVisible(bypassButton);
    
    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);
}

void EditorContent::resized()
{
    const juce::Rectangle baseDialBounds {0, 73, 80, 96};
    lowPassSlider.setBounds(baseDialBounds.withX (10));
    sizeSlider.setBounds(baseDialBounds.withX (108));
    dampSlider.setBounds(baseDialBounds.withX (202));
    widthSlider.setBounds(baseDialBounds.withX (402));
    mixSlider.setBounds(baseDialBounds.withX (500));
    highPassSlider.setBounds(baseDialBounds.withX (598));
    
    freezeButton.setBounds(baseDialBounds.withX(302));
    
    const auto bounds = getLocalBounds();
    bypassButton.setBounds(bounds.getWidth() - 70, 10, 60, 24);
    
    undoButton.setBounds(bounds.getWidth() - 140, 10, 60, 24);
    redoButton.setBounds(bounds.getWidth() - 210, 10, 60, 24);
    
}

bool EditorContent::keyPressed(const juce::KeyPress &k)
{
    if (k.isKeyCode(juce::KeyPress::tabKey) && hasKeyboardFocus (false))
    {
        sizeSlider.grabKeyboardFocus();
        return true;
    }
    
    return false;
}
