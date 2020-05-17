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
NumberSelector::NumberSelector(String componentName, SelectionType typeIn, SelectorStyle styleIn, Orientation orientationIn, Colour defaultTextColour)
	: selectionType(typeIn), selectorStyle(styleIn), orientation(orientationIn)
{
	setName(componentName);

	// Allow user text input
	if (selectionType == SelectionType::Range)
	{
		rangeValueLabel.reset(new Label());
		rangeValueLabel->addListener(this);
		rangeValueLabel->setEditable(false, true);
		addAndMakeVisible(rangeValueLabel.get());
	}
	// Essentially turns into a ComboBox wrapper
	else
	{
		listValueLabel.reset(new ComboBox());
		listValueLabel->addListener(this);
		addAndMakeVisible(listValueLabel.get());
	}

	float arrowDirection = (orientation == Horizontal) ? 0.0 : 0.25f;

	// TODO: Implement something else that can change colour
	incrementButton.reset(new ArrowButton("incrementButton", arrowDirection, defaultTextColour));
	addAndMakeVisible(incrementButton.get());
	incrementButton->addListener(this);

	decrementButton.reset(new ArrowButton("decrementButton", arrowDirection + 0.5f, defaultTextColour));
	addAndMakeVisible(decrementButton.get());
	decrementButton->addListener(this);

	titleLabel.reset(new Label(componentName + "Label", getName()));
	addChildComponent(titleLabel.get());

	valueFont.setDefaultMinimumHorizontalScaleFactor(0.5f);
	setupDefaultColours(defaultTextColour);
}

NumberSelector::NumberSelector(String componentName, SelectionType typeIn, Colour defaultTextColour)
	: NumberSelector(componentName, typeIn, TickBox, Horizontal, defaultTextColour)
{

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
	return titleLabel->isVisible();
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

	if (selectionType == SelectionType::Range && selectionRange.contains(newIndex))
	{
		setIndex(newIndex);
	}
	else if (selectionType == SelectionType::List && indexSelected < selectionList.size() - 1)
	{
		listValueLabel->setSelectedId(newIndex + 1);
	}
}

// Subtracts one from the index and updates the value
void NumberSelector::decrement()
{
	int newIndex = indexSelected - 1;

	if (selectionType == SelectionType::Range && selectionRange.contains(newIndex))
	{
		setIndex(newIndex);
	}
	else if (selectionType == SelectionType::List && indexSelected > 0)
	{
		listValueLabel->setSelectedId(newIndex + 1);
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
	titleLabel->setVisible(toShow);
	//repaint();
}

// Sets the value regardless of range/list. Index will be set to 0 if number is out of bounds
void NumberSelector::setValue(int valueIn, bool sendNotification)
{
	valueSelected = valueIn;
	updateIndexFromValue();

	if (selectionType == SelectionType::Range)
		updateTextBox();
	
	if (sendNotification)
		listeners.call(&NumberSelector::Listener::selectorValueChanged, this);
}

void NumberSelector::setIndex(int indexIn, bool sendNotification)
{
	indexSelected = indexIn;
	updateValueFromIndex();
	
	if (selectionType == SelectionType::Range)
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

	listValueLabel->clear();
	for (int i = 0; i < selectionList.size(); i++)
		listValueLabel->addItem(String(selectionList[i]), i + 1);
	
	if (sendNotification)
		listeners.call(&NumberSelector::Listener::selectorListChanged, this);

	if (updateValueAndIndex)
	{
		setIndex(jlimit(0, listIn.size() - 1, indexSelected), sendNotification);
	}
}

void NumberSelector::setListLookAndFeel(LookAndFeel* newLookAndFeel)
{
	if (listValueLabel.get())
		listValueLabel->setLookAndFeel(newLookAndFeel);
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

	// update colours here?
	
}

void NumberSelector::resized()
{
	if (selectorStyle == TickBox)
	{
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
		titleLabel->setFont(Font().withHeight(proportionOfHeight(0.2f)));
		int nameStringWidth = titleLabel->getFont().getStringWidth(getName()) * 1.05f;
		int nameStringHeight = titleLabel->getFont().getHeight();

		// TODO: handle vertical orientation

		float nameHeight = (namePlacementSelected == NamePlacement::AboveValue)
			? titleLabel->getPosition().y - proportionOfHeight(0.01f)
			: titleLabel->getPosition().y + titleLabel->getFont().getHeight() + proportionOfHeight(0.01f);
		
		titleLabel->setBounds(proportionOfWidth(0.5f) - nameStringWidth / 2.0f, nameHeight, nameStringWidth, nameStringHeight);
	}

	if (selectionType == SelectionType::Range)
	{
		rangeValueLabel->setBounds(proportionOfWidth(0.2f), 0, proportionOfWidth(0.6f), proportionOfHeight(1.0f));
		
		valueFont.setHeight(rangeValueLabel->getHeight());
		rangeValueLabel->setFont(valueFont);
		titleLabel->setJustificationType(Justification::centred);
	}
	else
	{
		listValueLabel->setBounds(proportionOfWidth(0.2f), 0, proportionOfWidth(0.6f), proportionOfHeight(1.0f));
	}
}

void NumberSelector::updateValueFromIndex()
{
	if (selectionType == SelectionType::Range)
	{
		valueSelected = indexSelected + selectionRange.getStart();
	}
	else
	{
		valueSelected = selectionList[indexSelected];
		listValueLabel->setText(String(valueSelected), dontSendNotification);
	}
}

void NumberSelector::updateIndexFromValue()
{
	indexSelected = (selectionType == SelectionType::Range)
		? valueSelected - selectionRange.getStart()
		: selectionList.indexOf(valueSelected);
}

void NumberSelector::updateTextBox()
{
	rangeValueLabel->setText(String(valueSelected), dontSendNotification);
}

void NumberSelector::buttonClicked(Button* buttonThatHasChanged)
{
	if (buttonThatHasChanged == incrementButton.get())
		increment();

	else if (buttonThatHasChanged == decrementButton.get())
		decrement();
}

void NumberSelector::comboBoxChanged(ComboBox* comboBox)
{
	setIndex(comboBox->getSelectedId() - 1);
}

void NumberSelector::labelTextChanged(Label* labelThatHasChanged)
{
	// Limit to valid range
	int newValue = selectionRange.clipValue(labelThatHasChanged->getText().getIntValue());
	setValue(newValue);
	DBG("NUMBERSELECTORLABEL: Value changed to " + String(getValue()));
}

void NumberSelector::editorShown(Label* label, TextEditor& editor)
{
	editor.addListener(this);
	editor.setJustification(Justification::centred);
	editor.applyFontToAllText(label->getFont().withHeight(editor.getHeight() * 0.95f));
}

void NumberSelector::editorHidden(Label* label, TextEditor& editor)
{
	editor.removeListener(this);
}

void NumberSelector::textEditorTextChanged(TextEditor& editor)
{
	String text = editor.getText();
	char lastChar = text.getLastCharacter();
	if (text.length() > 0 && (lastChar < 48 || lastChar > 57))
		editor.setText(text.replaceSection(text.length() - 1, 1, ""), false);
}

void NumberSelector::setupDefaultColours(Colour defaultTextColourIn)
{
	setColour(valueTextBackgroundColourId, Colour());
	setColour(valueTextBackgroundMouseOverColourId, Colours::white.withAlpha(0.1f));
	
	if (selectorStyle == SelectorStyle::TickBox)
		setColour(valueTextColourId, defaultTextColourIn);
	else if (selectorStyle == SelectorStyle::Belt)
		setColour(valueTextColourId, defaultTextColourIn.contrasting(1));
	
	setColour(valueTextColourMouseOverColourId, defaultTextColourIn.contrasting(0.1f));
	setColour(valueOutlineColourId, Colour());

	setColour(buttonBackgroundColourId, Colour());
	setColour(buttonBackgroundMouseOverColourId, Colours::white.withAlpha(0.1f));
	setColour(buttonBackgroundMouseDownColourId, Colours::lightgrey);
	setColour(buttonTextColourId, defaultTextColourIn);
	setColour(buttonTextMouseOverColourId, defaultTextColourIn.contrasting(0.1f));
	setColour(buttonTextMouseDownColourId, defaultTextColourIn.contrasting(0.25f));
	setColour(buttonOutlineColourId, Colour());

	setColour(beltBackgroundColorId, Colours::white);
	setColour(beltBuckleColourId, Colours::darkgrey);

	titleLabel->setColour(Label::ColourIds::backgroundColourId, findColour(valueTextBackgroundColourId));
	titleLabel->setColour(Label::ColourIds::textColourId, findColour(valueTextColourId));
	titleLabel->setColour(Label::ColourIds::outlineColourId, findColour(valueOutlineColourId));

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