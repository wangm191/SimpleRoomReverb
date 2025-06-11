/*
  ==============================================================================

    undoButton.cpp
    Created: 10 Jun 2025 12:32:38pm
    Author:  Myles Wang

  ==============================================================================
*/

#include "UndoManagerButton.h"
#include "UseColors.h"

UndoManagerButton::UndoManagerButton(juce::UndoManager& um, ActionType actionType)
    : undoManager(um), type(actionType)
{
    setButtonText(type == ActionType::Undo ? "Undo" : "Redo");
    
    setColour(juce::TextButton::buttonColourId, UseColors::green);
    setColour(juce::TextButton::textColourOffId, UseColors::blue);

    onClick = [this]()
    {
        if (type == ActionType::Undo && undoManager.canUndo())
            undoManager.undo();
        else if (type == ActionType::Redo && undoManager.canRedo())
            undoManager.redo();
    };

    startTimerHz(10); // Update enable/disable status regularly
}

void UndoManagerButton::timerCallback()
{
    updateButtonState();
}

void UndoManagerButton::updateButtonState()
{
    if (type == ActionType::Undo)
        setEnabled(undoManager.canUndo());
    else
        setEnabled(undoManager.canRedo());
}

