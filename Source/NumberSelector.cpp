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
NumberSelector::NumberSelector(String componentName, SelectionType typeIn, SelectorStyle styleIn, Orientation orientationIn)
	: selectionType(typeIn), selectorStyle(styleIn), orientation(orientationIn)
{
	setName(componentName);

	valueLabel.reset(new Label());
	addAndMakeVisible(valueLabel.get());

	float arrowDirection = (orientation == Horizontal) ? 0.0 : 0.25f;

	incrementButton.reset(new ArrowButton("incrementButton", arrowDirection, Colours::white));
	addAndMakeVisible(incrementButton.get());
	incrementButton->addListener(this);

	decrementButton.reset(new ArrowButton("decrementButton", arrowDirection + 0.5f, Colours::white));
	addAndMakeVisible(decrementButton.get());
	decrementButton->addListener(this);

	nameLabel.reset(new Label(componentName + "Label", getName()));
	addChildComponent(nameLabel.get());

	setupDefaultColours();
}

NumberSelector::~NumberSelector()
{
	listeners.clear();
}

NumberSelector::SelectionType NumberSelector::getSelectionType() const
{
	return selectionType;
}

NumberSelector::SelectorStyle NumberSelector::getSelectorStyle() const
{
	return selectorStyle;
}

NumberSelector::Orientation NumberSelector::getOrientation() const
{
	return orientation;
}

NumberSelector::NamePlacement NumberSelector::getNamePlacement() const
{
	return namePlacementSelected;
}

bool NumberSelector::isShowingName() const
{
	return nameLabel->isVisible();
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

void NumberSelector::setSelectorStyle(NumberSelector::SelectorStyle styleIn)
{
	selectorStyle = styleIn;
	
	if (selectorStyle == SelectorStyle::TickBox)
		setColour(valueTextColourId, Colours::white);
	else if (selectorStyle == SelectorStyle::Belt)
		setColour(valueTextColourId, Colours::black);

	resized();
}

void NumberSelector::setOrientation(Orientation orientationIn)
{
	orientation = orientationIn;
	resized();
}

void NumberSelector::setNamePlacement(NamePlacement placementIn)
{
	namePlacementSelected = placementIn;
	resized();
}

void NumberSelector::showNameLabel(bool toShow)
{
	nameLabel->setVisible(toShow);
	//repaint();
}

// Sets the value regardless of range/list. Index will be set to 0 if number is out of bounds
void NumberSelector::setValue(int valueIn, bool sendNotification)
{
	valueSelected = valueIn;
	updateIndexFromValue();
	updateTextBox();
	if (sendNotification)
		listeners.call(&NumberSelector::Listener::selectorValueChanged, this);
}

void NumberSelector::setIndex(int indexIn, bool sendNotification)
{
	indexSelected = indexIn;
	updateValueFromIndex();
	updateTextBox();
	if (sendNotification)
		listeners.call(&NumberSelector::Listener::selectorValueChanged, this);
}

void NumberSelector::setRange(IntRange rangeIn, bool updateValueAndIndex, bool sendNotification)
{
	selectionRange = rangeIn;
	
	if (sendNotification)
		listeners.call(&NumberSelector::Listener::selectorRangeChanged, this);
	
	if (updateValueAndIndex)
		setIndex(rangeIn.clipValue(indexSelected), sendNotification);
}

void NumberSelector::setRange(int min, int max, bool updateValueAndIndex, bool sendNotification)
{
	setRange(IntRange(min, max), updateValueAndIndex, sendNotification);
}

void NumberSelector::setList(IntList listIn, bool updateValueAndIndex, bool sendNotification)
{
	selectionList = listIn;
	
	if (sendNotification)
		listeners.call(&NumberSelector::Listener::selectorListChanged, this);

	if (updateValueAndIndex)
		setIndex(jlimit(0, listIn.size() - 1, indexSelected), sendNotification);
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

	if (selectorStyle == SelectorStyle::Belt)
	{
		g.setColour(findColour(ColourIds::beltBackgroundColorId));
		g.fillRect(getBounds());
	}
}

void NumberSelector::resized()
{
	if (selectorStyle == TickBox)
	{
		valueLabel->setBounds(proportionOfWidth(0.2f), 0, proportionOfWidth(0.6f), proportionOfHeight(1.0f));

		if (orientation == Horizontal)
		{
			decrementButton->setBounds(0, proportionOfHeight(3.0f / 8.0f), proportionOfWidth(0.2f), proportionOfHeight(1.0f / 3.0f));
			incrementButton->setBounds(proportionOfWidth(0.8f), proportionOfHeight(3.0f / 8.0f), proportionOfWidth(0.2f), proportionOfHeight(1.0f / 3.0f));
		}
		else
		{
			decrementButton->setBounds(0, proportionOfHeight(1.2f), proportionOfWidth(0.2f), proportionOfHeight(1.0f / 3.0f));
			incrementButton->setBounds(0, -proportionOfHeight(1.0f / 3.0f), proportionOfWidth(3.0f / 8.0f), proportionOfHeight(1.0f / 3.0f));
		}
	}

	if (isShowingName())
	{
		nameLabel->setFont(Font().withHeight(proportionOfHeight(0.2f)));
		int nameStringWidth = nameLabel->getFont().getStringWidth(getName()) * 1.05f;
		int nameStringHeight = nameLabel->getFont().getHeight();

		// TODO: handle vertical orientation

		float nameHeight = (namePlacementSelected == NamePlacement::AboveValue)
			? valueLabel->getPosition().y - proportionOfHeight(0.01f)
			: valueLabel->getPosition().y + valueLabel->getFont().getHeight() + proportionOfHeight(0.01f);		
		
		nameLabel->setBounds(proportionOfWidth(0.5f) - nameStringWidth / 2.0f, nameHeight, nameStringWidth, nameStringHeight);
	}

	valueFont.setHeight(proportionOfHeight(0.9f));
	valueLabel->setFont(valueFont);
	valueLabel->setJustificationType(Justification::centred);	
}

void NumberSelector::updateValueFromIndex()
{
	valueSelected = (selectionType == SelectionType::Range)
		? indexSelected + selectionRange.getStart()
		: selectionList[indexSelected];
}

void NumberSelector::updateIndexFromValue()
{
	indexSelected = (selectionType == SelectionType::Range)
		? valueSelected - selectionRange.getStart()
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
	
	if (selectorStyle == SelectorStyle::TickBox)
		setColour(valueTextColourId, Colours::white);
	else if (selectorStyle == SelectorStyle::Belt)
		setColour(valueTextColourId, Colours::black);
	
	setColour(valueTextColourMouseOverColourId, Colours::white.withAlpha(0.1f));
	setColour(valueOutlineColourId, Colour());

	setColour(buttonBackgroundColourId, Colour());
	setColour(buttonBackgroundMouseOverColourId, Colours::white.withAlpha(0.1f));
	setColour(buttonBackgroundMouseDownColourId, Colours::lightgrey);
	setColour(buttonTextColourId, Colours::white);
	setColour(buttonTextMouseOverColourId, Colours::white.withAlpha(0.1f));
	setColour(buttonTextMouseDownColourId, Colours::lightgrey);
	setColour(buttonOutlineColourId, Colour());

	setColour(beltBackgroundColorId, Colours::white);
	setColour(beltBuckleColourId, Colours::darkgrey);

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