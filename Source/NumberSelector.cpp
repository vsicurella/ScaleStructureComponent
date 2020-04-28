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
	textValue.reset(new Label());
	addAndMakeVisible(textValue.get());

	incrementButton.reset(new TextButton(">"));
	addAndMakeVisible(incrementButton.get());
	incrementButton->addListener(this);

	decrementButton.reset(new TextButton("<"));
	addAndMakeVisible(decrementButton.get());
	decrementButton->addListener(this);
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

	textValue->setBounds(proportionOfWidth(0.25f), 0, proportionOfWidth(0.5f), proportionOfHeight(1.0f));
	decrementButton->setBounds(0, 0, proportionOfWidth(0.25f), proportionOfHeight(1.0f));
	incrementButton->setBounds(proportionOfWidth(0.75f), 0, proportionOfWidth(0.25f), proportionOfHeight(1.0f));

	valueFont.setHeight(proportionOfHeight(0.9f));
	textValue->setFont(valueFont);
	textValue->setJustificationType(Justification::centred);
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
	textValue->setText(String(valueSelected), dontSendNotification);
}

void NumberSelector::buttonClicked(Button* buttonThatHasChanged)
{
	if (buttonThatHasChanged == incrementButton.get())
		increment();

	else if (buttonThatHasChanged == decrementButton.get())
		decrement();
}