/*
  ==============================================================================

    undoButton.h
    Created: 10 Jun 2025 12:32:38pm
    Author:  Myles Wang

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_data_structures/juce_data_structures.h>

class UndoManagerButton : public juce::TextButton,
                          private juce::Timer
{
public:
    enum class ActionType { Undo, Redo };

    UndoManagerButton(juce::UndoManager& um, ActionType actionType);

private:
    void timerCallback() override;
    void updateButtonState();

    juce::UndoManager& undoManager;
    ActionType type;
};
