/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "ScaleStructureComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
ScaleStructureComponent::ScaleStructureComponent (ScaleStructure& scaleStructureIn, Array<Colour>& colourTableIn)
    : scaleStructure(scaleStructureIn), colourTable(colourTableIn)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    circleComponent.reset (new GroupingCircle (scaleStructure.getDegreeGroupingsReference(), colourTable));
    addAndMakeVisible (circleComponent.get());
    circleComponent->setName ("circleComponent");

    generatorSlider.reset (new NumberSelector ("Generator", NumberSelector::SelectionType::List));
    addAndMakeVisible (generatorSlider.get());
    generatorSlider->setName ("Generator");

    periodSlider.reset (new NumberSelector ("Period"));
    addAndMakeVisible (periodSlider.get());
    periodSlider->setName ("Period");

    generatorValueLbl.reset (new Label ("generatorValueLbl",
                                        TRANS("700 cents")));
    addAndMakeVisible (generatorValueLbl.get());
    generatorValueLbl->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    generatorValueLbl->setJustificationType (Justification::centred);
    generatorValueLbl->setEditable (false, false, false);
    generatorValueLbl->setColour (TextEditor::textColourId, Colours::black);
    generatorValueLbl->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    stepSizePatternLbl.reset (new Label ("stepSizePatternLbl",
                                         TRANS("LLsLLLs\n")));
    addAndMakeVisible (stepSizePatternLbl.get());
    stepSizePatternLbl->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    stepSizePatternLbl->setJustificationType (Justification::centred);
    stepSizePatternLbl->setEditable (false, false, false);
    stepSizePatternLbl->setColour (TextEditor::textColourId, Colours::black);
    stepSizePatternLbl->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
	generatorLookAndFeel.reset(new TransparentDropDown());
	// TODO: add SSC colour ids
	//generatorLookAndFeel.setBaseColour(findColour(ColourIds::backgroundColourId));
	generatorLookAndFeel->setColour(PopupMenu::ColourIds::backgroundColourId, Colour());
	generatorSlider->setLookAndFeel(generatorLookAndFeel.get());

	offsetLabel.reset(new Label("offsetLabel", "Offset\n0"));
	offsetLabel->setJustificationType(Justification::centred);
	offsetLabel->setColour(Label::ColourIds::textColourId, Colours::white);
	addAndMakeVisible(offsetLabel.get());

	sizeSelector.reset(new NumberSelector("Scale Size", NumberSelector::SelectionType::List, colourTable[0].contrasting(0.8f)));
	//sizeSelector->setColour(NumberSelector::ColourIds::valueTextColourId, colourTable[0].contrasting(1.0));
	/*sizeSelector->setColour(NumberSelector::ColourIds::, colourTable[0].contrasting(1.0));*/
	addAndMakeVisible(sizeSelector.get());

	sizeLookAndFeel.reset(new TransparentDropDown());
	sizeLookAndFeel->setBaseColour(colourTable.getReference(0));
	sizeLookAndFeel->setColour(PopupMenu::ColourIds::backgroundColourId, Colour());
	sizeLookAndFeel->setColour(ComboBox::ColourIds::textColourId, sizeSelector->findColour(NumberSelector::ColourIds::valueTextColourId));
	sizeSelector->setLookAndFeel(sizeLookAndFeel.get());


	// TODO: add component colour ids
	//periodFactorLookAndFeel.setBaseColour(findColour(ColourIds::backgroundColourId));
	periodFactorButton.reset(new ShapeButton("periodFactorButton", Colours::white, Colours::white.contrasting(0.125f), Colours::white.contrasting(0.25f)));
	periodFactorButton->setTooltip("Number of periods.");
	addAndMakeVisible(periodFactorButton.get());

	periodFactorLookAndFeel.reset(new TransparentDropDown());
	periodFactorMenu.setLookAndFeel(periodFactorLookAndFeel.get());

	circle = dynamic_cast<GroupingCircle*>(circleComponent.get());

	periodSlider->showNameLabel();
	generatorSlider->showNameLabel();
	sizeSelector->showNameLabel();
    //[/UserPreSize]

    setSize (800, 800);


    //[Constructor] You can add your own custom stuff here..

	// Set up default values, then set up listening
	periodSlider->setRange(5, 275, true, false);
	periodSlider->setValue(scaleStructure.getPeriod());

	updateGenerators();

	scaleStructure.resetToPeriod(12);
	scaleStructure.setGeneratorIndex(scaleStructure.getSuggestedGeneratorIndex());
	scaleStructure.setSizeIndex(scaleStructure.getSuggestedSizeIndex());
	scaleStructure.setGeneratorOffset(1);

	periodFactors = scaleStructure.getPeriodFactors();
	if (periodFactors.size() == 1)
		periodFactorButton->setVisible(false);

	generatorSlider->setList(scaleStructure.getValidGenerators());
	generatorSlider->setIndex(scaleStructure.getGeneratorIndex());

	updateScaleSizes();

	circleOffset = &circle->getOffsetValue();
	*circleOffset = 1;
	circle->setOffsetLimit(scaleStructure.getScaleSize() - 1);
	offsetLabel->setText("Offset\n" + String((int)circleOffset->getValue()), dontSendNotification);

	periodSlider->addListener(this);
	generatorSlider->addListener(this);

	circleOffset->addListener(this);
	sizeSelector->addListener(this);
	periodFactorButton->addListener(this);

	circle->updatePeriod(scaleStructure.getPeriod());
	circle->updateGenerator();
    //[/Constructor]
}

ScaleStructureComponent::~ScaleStructureComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    circleComponent = nullptr;
    generatorSlider = nullptr;
    periodSlider = nullptr;
    generatorValueLbl = nullptr;
    stepSizePatternLbl = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
	sizeSelector = nullptr;
	periodFactorMenu.setLookAndFeel(nullptr);
    //[/Destructor]
}

//==============================================================================
void ScaleStructureComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..

	// Offset Label arrows
	g.setColour(Colours::white);
	PathStrokeType strokeType(1.0f);
	g.strokePath(offsetArrows, strokeType);
    //[/UserPaint]
}

void ScaleStructureComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    circleComponent->setBounds (0, 0, proportionOfWidth (1.0000f), proportionOfHeight (1.0000f));
    generatorSlider->setBounds (proportionOfWidth (0.5011f) - (proportionOfWidth (0.2500f) / 2), proportionOfHeight (0.4658f), proportionOfWidth (0.2500f), proportionOfHeight (0.1492f));
    periodSlider->setBounds (proportionOfWidth (0.5041f) - (proportionOfWidth (0.2500f) / 2), proportionOfHeight (0.3064f), proportionOfWidth (0.2500f), proportionOfHeight (0.1492f));
    generatorValueLbl->setBounds (proportionOfWidth (0.3607f) - (103 / 2), proportionOfHeight (0.7050f), 103, 24);
    stepSizePatternLbl->setBounds (proportionOfWidth (0.6404f) - (96 / 2), proportionOfHeight (0.7050f), 96, 24);
    //[UserResized] Add your own custom resize handling here..

	// TODO: implement (probably ex-projucer) this so that the bounds don't have to be set twice
	periodSlider->setCentrePosition(circle->getIntPointFromCenter(circle->getInnerRadius() * 0.4f, 0));
	generatorSlider->setCentrePosition(circle->getIntPointFromCenter(circle->getInnerRadius() * 0.125f, float_Pi));

	generatorValueLbl->setCentrePosition(circle->getIntPointFromCenter(circle->getInnerRadius() * 2.0f / 3.0f, float_Pi * 11.0f / 8.0f));
	stepSizePatternLbl->setCentrePosition(circle->getIntPointFromCenter(circle->getInnerRadius() * 2.0f / 3.0f, float_Pi * 5.0f / 8.0f));

	offsetLabel->setFont(Font().withHeight(getHeight() / 48.0f));
	offsetLabel->setSize(offsetLabel->getFont().getStringWidth("Offset") * 2, offsetLabel->getFont().getHeight() * 3);
	offsetLabel->setCentrePosition(circle->getIntPointFromCenter(circle->getInnerRadius() * 7.0f / 8.0f, 0));

	sizeSelector->setSize(proportionOfWidth(0.175f), (circle->getOuterRadius() - circle->getMiddleRadius()) * 7.0f / 8.0f);
	sizeSelector->setCentrePosition(circle->getIntPointFromCenter((circle->getOuterRadius() + circle->getMiddleRadius()) / 2.025f, 0));

	// TODO: add other part of arrow / improve drawing
	offsetArrows.clear();
	GroupingCircle::addArcToPath(offsetArrows, circle->getInnerCircleBounds().reduced(circle->getInnerRadius() / 13.0f), float_Pi / 24, float_Pi / 12, true);
	offsetArrows.lineTo(circle->getFloatPointFromCenter(circle->getInnerRadius() * 13.0f / 14.0f, float_Pi / 14));
	GroupingCircle::addArcToPath(offsetArrows, circle->getInnerCircleBounds().reduced(circle->getInnerRadius() / 13.0f), -float_Pi / 24, -float_Pi / 12, true);
	offsetArrows.lineTo(circle->getFloatPointFromCenter(circle->getInnerRadius() * 13.0f / 14.0f, -float_Pi / 14));

	float periodFBtnSize = periodSlider->getHeight() / 8.0f;
	periodFactorButtonShape.clear();
	periodFactorButtonShape.addEllipse(periodSlider->getRight(), periodSlider->getY(), periodFBtnSize, periodFBtnSize);
	periodFactorButton->setShape(periodFactorButtonShape, true, true, true);
	periodFactorButton->setTopLeftPosition(periodSlider->getPosition().translated(periodSlider->getWidth() * 4 / 5.0f, 0));
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void ScaleStructureComponent::paintOverChildren(Graphics& g)
{
	// Period Factor label
	if (periodFactorButton->isVisible())
	{
		g.setColour(Colours::black);
		g.setFont(periodFactorButton->getHeight() * 7 / 8.0f );
		g.drawFittedText(String(scaleStructure.getPeriodFactor()), periodFactorButton->getBounds().translated(0, -1), Justification::centred, 1);
	}
}

void ScaleStructureComponent::buttonClicked(Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == periodFactorButton.get())
	{
		periodFactorMenu.clear();
		for (int i = 0; i < periodFactors.size(); i++)
		{
			periodFactorMenu.addItem(i + 1, String(periodFactors[i]), true, i == scaleStructure.getPeriodFactorIndex());
		}

		PopupMenu::Options options = PopupMenu::Options()
			.withMinimumWidth(periodSlider->getHeight() / 4)
			.withStandardItemHeight(periodSlider->getHeight() / 4)
			.withTargetComponent(periodFactorButton.get());

		periodFactorMenu.showMenuAsync(options, [=](int choice)
		{
			updatePeriodFactor(choice - 1);
		});
	}
}

void ScaleStructureComponent::selectorValueChanged(NumberSelector* selectorThatHasChanged)
{
	if (selectorThatHasChanged == periodSlider.get())
	{
		periodSelected = periodSlider->getValue();
		DBG("SSC: Period changed to: " + String(periodSelected));

		scaleStructure.resetToPeriod(periodSelected);
		circle->updatePeriod(periodSelected);

		periodFactors = scaleStructure.getPeriodFactors();
		periodFactorSelected = 0;

		if (periodFactors.size() > 1)
			periodFactorButton->setVisible(true);
		else
			periodFactorButton->setVisible(false);

		updateGenerators();
	}

	else if (selectorThatHasChanged == generatorSlider.get())
	{
		generatorSelected = generatorSlider->getIndex();
		scaleStructure.setGeneratorIndex(generatorSelected);
		circle->updateGenerator();
		DBG("SSC: Generator changed to: " + String(generatorSlider->getValue()));

		float cents = roundf(log2(pow(2, (double)scaleStructure.getGenerator() / periodSelected)) * 1200000) / 1000.0f;
		generatorValueLbl->setText(String(cents) + " cents", dontSendNotification);

		updateScaleSizes();
	}

	else if (selectorThatHasChanged == sizeSelector.get())
	{
		// Prevent fractional periods from suggesting index 0
		//if (sizeSelector->getSelectedId() == 0)
		//	sizeSelector->setSelectedId(1, dontSendNotification);

		scaleStructure.setSizeIndex(sizeSelector->getIndex() + 1);
		DBG("SSC: Size changed to: " + String(scaleStructure.getScaleSize()));

		circle->setOffsetLimit(scaleStructure.getScaleSize() - 1);
		circle->updateGenerator();

		stepSizePatternLbl->setText(scaleStructure.getLsSteps(), dontSendNotification);
	}

	// TODO: Figure out a way to only call this once per change
	sendChangeMessage();
}

void ScaleStructureComponent::valueChanged(Value& valueThatHasChanged)
{
	if (valueThatHasChanged.refersToSameSourceAs(*circleOffset))
	{
		//offsetSlider->setValue(circleOffset->getValue());
		scaleStructure.setGeneratorOffset((int) circleOffset->getValue());
		circle->updateGenerator();

		DBG("SSC: Generator Offset changed to: " + String((int) circleOffset->getValue()));
		stepSizePatternLbl->setText(scaleStructure.getLsSteps(), dontSendNotification);
		offsetLabel->setText("Offset\n" + String((int)circleOffset->getValue()), dontSendNotification);

		sendChangeMessage();
	}
}

void ScaleStructureComponent::updateGenerators()
{
	generatorSlider->setList(scaleStructure.getValidGenerators(), false, false);
	generatorSlider->setIndex(scaleStructure.getSuggestedGeneratorIndex());
}

void ScaleStructureComponent::updateScaleSizes()
{
	Array<int> sizes = scaleStructure.getScaleSizes();
	sizes.remove(0);
	if (scaleStructure.getPeriodFactor() > 1)
	{
		for (int i = 0; i < sizes.size(); i++)
		{
			sizes.set(i, sizes[i] * scaleStructure.getPeriodFactor());
		}
	}

	sizeSelector->setList(sizes);
	sizeSelector->setIndex(jlimit(0, sizes.size(), scaleStructure.getSuggestedSizeIndex() - 1), sendNotificationSync);
}

void ScaleStructureComponent::updatePeriodFactor(int factorIndexIn)
{
	if (factorIndexIn > -1)
	{
		periodFactorSelected = factorIndexIn;

		scaleStructure.setPeriodFactorIndex(periodFactorSelected);
		updateGenerators();
	}
	else
		periodFactorSelected = 0;

	repaint();
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ScaleStructureComponent"
                 componentName="" parentClasses="public Component, public ChangeBroadcaster, private NumberSelector::Listener, private Value::Listener, private Button::Listener"
                 constructorParams="ScaleStructure&amp; scaleStructureIn, Array&lt;Colour&gt;&amp; colourTableIn"
                 variableInitialisers="scaleStructure(scaleStructureIn), colourTable(colourTableIn)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="800" initialHeight="800">
  <BACKGROUND backgroundColour="ff323e44"/>
  <GENERICCOMPONENT name="circleComponent" id="ec9c5dc09c2f91cf" memberName="circleComponent"
                    virtualName="" explicitFocusOrder="0" pos="0 0 100% 100%" class="GroupingCircle"
                    params="scaleStructure.getDegreeGroupingsReference(), colourTable"/>
  <GENERICCOMPONENT name="Generator" id="efbe5586805bc62b" memberName="generatorSlider"
                    virtualName="NumberSelector" explicitFocusOrder="0" pos="50.113%c 46.583% 25% 14.92%"
                    class="Component" params="&quot;Generator&quot;, NumberSelector::SelectionType::List"/>
  <GENERICCOMPONENT name="Period" id="39f9599ebb9952a" memberName="periodSlider"
                    virtualName="NumberSelector" explicitFocusOrder="0" pos="50.413%c 30.638% 25% 14.92%"
                    class="Component" params="&quot;Period&quot;"/>
  <LABEL name="generatorValueLbl" id="7250d3d0fa11afcf" memberName="generatorValueLbl"
         virtualName="" explicitFocusOrder="0" pos="36.074%c 70.501% 103 24"
         edTextCol="ff000000" edBkgCol="0" labelText="700 cents" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="stepSizePatternLbl" id="b4e52c793121b24" memberName="stepSizePatternLbl"
         virtualName="" explicitFocusOrder="0" pos="64.039%c 70.501% 96 24"
         edTextCol="ff000000" edBkgCol="0" labelText="LLsLLLs&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

