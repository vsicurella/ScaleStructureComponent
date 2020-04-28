/*
  ==============================================================================

    NumberSelector.h
    Created: 27 Apr 2020 11:22:44am
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
	A component similar to Slider's IncDecButtons style that allows for ranges 
	of positive integers, or an arbtriary list of integers. 
	The value text box is centered between the buttons.
*/
class NumberSelector :	public Component,
						private Button::Listener
{
public:

	typedef juce::Range<int> IntRange;
	typedef Array<int> IntList;

	enum SelectionType
	{
		Range = 0,
		List
	};

	enum ColourIds
	{
		valueTextBackgroundColourId = 0x1001200,
		valueTextBackgroundMouseOverColourId = 0x1001210,
		valueTextColourId = 0x1001300,
		valueTextColourMouseOverColourId = 0x1001310,
		valueOutlineColourId = 0x1001400,

		buttonBackgroundColourId = 0x1001500,
		buttonBackgroundMouseOverColourId = 0x1001510,
		buttonTextColourId = 0x1001600,
		buttonTextMouseOverColourId = 0x1001610,
		buttonOutlineColourId = 0x1001700
	};

public:
	NumberSelector();
	~NumberSelector();

	int getSelectionType() const;

	int getValue() const;

	int getIndex() const;

	// Returns the range of available integers; may be default if using a List Selection Type
	IntRange getRange() const;

	// Returns the list of available integers; may be default if using a Range Selection Type
	IntList getList() const;

	// Adds one to the index and updates the value
	void increment();

	// Subtracts one from the index and updates the value
	void decrement();

	void setSelectionType(SelectionType typeIn);

	// Sets the value regardless of range/list. Index will be set to 0 if number is out of bounds
	void setValue(int valueIn);

	void setIndex(int indexIn);

	void setRange(IntRange rangeIn, bool updateValueAndIndex = true);
	void setRange(int min, int max, bool updateValueAndIndex = true);

	void setList(IntList listIn, bool updateValueAndIndex = true);

	void paint(Graphics&) override;

	void resized() override;

	class Listener
	{
	public:
		virtual ~Listener() {}

		// Called when value changes
		virtual void selectorValueChanged(NumberSelector* selectorThatHasChanged) = 0;

		// Called when range changes
		virtual void selectorRangeChanged(NumberSelector* selectorThatHasChanged) {};

		// Called when list changes
		virtual void selectorListChanged(NumberSelector* selectorThatHasChanged) {};
	};

	void addListener(Listener* listenerIn);

	void removeListener(Listener* listenerIn);

protected:
	ListenerList<Listener> listeners;

private:
	std::unique_ptr<TextButton> incrementButton;
	std::unique_ptr<TextButton> decrementButton;
	std::unique_ptr<Label> textValue;

	int valueSelected = 0;
	int indexSelected = 0;
	SelectionType selectionType = SelectionType::Range;

	IntRange selectionRange;
	IntList selectionList;

	Font valueFont;

	void updateValueFromIndex();
	void updateIndexFromValue();
	void updateTextBox();

	void buttonClicked(Button* buttonThatHasChanged) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NumberSelector)
};

