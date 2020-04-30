/*
  ==============================================================================

    NumberSelector.cpp
    Created: 27 Apr 2020 11:22:44am
    Author:  Vincenzo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "NumberSelector.h"

//==============================================================================
NumberSelector::NumberSelector()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
	valueLabel.reset(new Label());
	addAndMakeVisible(valueLabel.get());

	incrementButton.reset(new ArrowButton("incrementButton", 0.0f, Colours::white));
	addAndMakeVisible(incrementButton.get());
	incrementButton->addListener(this);

	decrementButton.reset(new ArrowButton("decrementButton", 0.5f, Colours::white));
	addAndMakeVisible(decrementButton.get());
	decrementButton->addListener(this);

	setupDefaultColours();
}

NumberSelector::~NumberSelector()
{
	listeners.clear();
}

int NumberSelector::getSelectionType() const
{
	return selectionType;
}

int NumberSelector::getValue() const
{
	return valueSelected;
}

int NumberSelector::getIndex() const
{
	return indexSelected;
}

// Returns the range of available integers; may be default if using a List Selection Type
NumberSelector::IntRange NumberSelector::getRange() const
{
	return selectionRange;
}

// Returns the list of available integers; may be default if using a Range Selection Type
NumberSelector::IntList NumberSelector::getList() const
{
	return selectionList;
}

// Adds one to the index and updates the value
void NumberSelector::increment()
{
	int newIndex = indexSelected + 1;

	if (selectionType == SelectionType::Range && selectionRange.contains(newIndex) ||
		selectionType == SelectionType::List && indexSelected < selectionList.size() - 1)
	{
		setIndex(newIndex);
	}
}

// Subtracts one from the index and updates the value
void NumberSelector::decrement()
{
	int newIndex = indexSelected - 1;

	if (selectionType == SelectionType::Range && selectionRange.contains(newIndex) ||
		selectionType == SelectionType::List && indexSelected > 0)
	{
		setIndex(newIndex);
	}
}

void NumberSelector::setSelectionType(SelectionType typeIn)
{
	selectionType = typeIn;
	updateValueFromIndex();
	setValue(valueSelected);
}

// Sets the value regardless of range/list. Index will be set to 0 if number is out of bounds
void NumberSelector::setValue(int valueIn)
{
	valueSelected = valueIn;
	updateIndexFromValue();
	updateTextBox();
	listeners.call(&NumberSelector::Listener::selectorValueChanged, this);
}

void NumberSelector::setIndex(int indexIn)
{
	indexSelected = indexIn;
	updateValueFromIndex();
	updateTextBox();
	listeners.call(&NumberSelector::Listener::selectorValueChanged, this);
}

void NumberSelector::setRange(IntRange rangeIn, bool updateValueAndIndex)
{
	selectionRange = rangeIn;
	listeners.call(&NumberSelector::Listener::selectorRangeChanged, this);

	setIndex(rangeIn.clipValue(indexSelected));
}

void NumberSelector::setRange(int min, int max, bool updateValueAndIndex)
{
	setRange(IntRange(min, max), updateValueAndIndex);
}

void NumberSelector::setList(IntList listIn, bool updateValueAndIndex)
{
	selectionList = listIn;
	listeners.call(&NumberSelector::Listener::selectorListChanged, this);

	setIndex(jlimit(0, listIn.size() - 1, indexSelected));
}

void NumberSelector::addListener(Listener* listenerIn)
{
	listeners.add(listenerIn);
}

void NumberSelector::removeListener(Listener* listenerIn)
{
	listeners.remove(listenerIn);
}

void NumberSelector::paint (Graphics& g)
{
	g.fillAll(Colour());
}

void NumberSelector::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

	valueLabel->setBounds(proportionOfWidth(0.2f), 0, proportionOfWidth(0.6f), proportionOfHeight(1.0f));
	decrementButton->setBounds(0, proportionOfHeight(3.0f / 8.0f), proportionOfWidth(0.2f), proportionOfHeight(1.0f / 3.0f));
	incrementButton->setBounds(proportionOfWidth(0.8f), proportionOfHeight(3.0f / 8.0f), proportionOfWidth(0.2f), proportionOfHeight(1.0f / 3.0f));

	valueFont.setHeight(proportionOfHeight(0.9f));
	valueLabel->setFont(valueFont);
	valueLabel->setJustificationType(Justification::centred);
}

void NumberSelector::updateValueFromIndex()
{
	valueSelected = (selectionType == SelectionType::Range)
		? indexSelected
		: selectionList[indexSelected];
}

void NumberSelector::updateIndexFromValue()
{
	indexSelected = (selectionType == SelectionType::Range)
		? valueSelected
		: selectionList.indexOf(valueSelected);
}

void NumberSelector::updateTextBox()
{
	valueLabel->setText(String(valueSelected), dontSendNotification);
}

void NumberSelector::buttonClicked(Button* buttonThatHasChanged)
{
	if (buttonThatHasChanged == incrementButton.get())
		increment();

	else if (buttonThatHasChanged == decrementButton.get())
		decrement();
}

void NumberSelector::setupDefaultColours()
{
	setColour(valueTextBackgroundColourId, Colour());
	setColour(valueTextBackgroundMouseOverColourId, Colours::white.withAlpha(0.1f));
	setColour(valueTextColourId, Colours::white);
	setColour(valueTextColourMouseOverColourId, Colours::white.withAlpha(0.1f));
	setColour(valueOutlineColourId, Colour());

	setColour(buttonBackgroundColourId, Colour());
	setColour(buttonBackgroundMouseOverColourId, Colours::white.withAlpha(0.1f));
	setColour(buttonBackgroundMouseDownColourId, Colours::lightgrey);
	setColour(buttonTextColourId, Colours::white);
	setColour(buttonTextMouseOverColourId, Colours::white.withAlpha(0.1f));
	setColour(buttonTextMouseDownColourId, Colours::lightgrey);
	setColour(buttonOutlineColourId, Colour());

	valueLabel->setColour(Label::ColourIds::backgroundColourId, findColour(valueTextBackgroundColourId));
	valueLabel->setColour(Label::ColourIds::textColourId, findColour(valueTextColourId));
	valueLabel->setColour(Label::ColourIds::outlineColourId, findColour(valueOutlineColourId));

	//incrementButton->setColour(TextButton::ColourIds::buttonColourId, findColour(buttonBackgroundColourId));
	//incrementButton->setColour(TextButton::ColourIds::buttonOnColourId, findColour(buttonBackgroundMouseDownColourId));
	//incrementButton->setColour(TextButton::ColourIds::textColourOffId, findColour(buttonTextColourId));
	//incrementButton->setColour(TextButton::ColourIds::textColourOnId, findColour(buttonTextMouseDownColourId));
	//

	//decrementButton->setColour(TextButton::ColourIds::buttonColourId, findColour(buttonBackgroundColourId));
	//decrementButton->setColour(TextButton::ColourIds::buttonOnColourId, findColour(buttonBackgroundMouseDownColourId));
	//decrementButton->setColour(TextButton::ColourIds::textColourOffId, findColour(buttonTextColourId));
	//decrementButton->setColour(TextButton::ColourIds::textColourOnId, findColour(buttonTextMouseDownColourId));
}