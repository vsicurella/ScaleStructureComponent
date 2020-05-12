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

    circleComponent.reset (new GroupingCircle (scaleStructure.getGeneratorChainReference(), scaleStructure.getGroupingSizesReference(), colourTable));
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

    periodFactorSelector.reset (new NumberSelector ("Period\nFactor", NumberSelector::SelectionType::List, NumberSelector::SelectorStyle::TickBox, NumberSelector::Orientation::Vertical));
    addAndMakeVisible (periodFactorSelector.get());
    periodFactorSelector->setName ("Period Factor");

    scaleSizeSelector.reset (new NumberSelector ("Scale Size", NumberSelector::SelectionType::List));
    addAndMakeVisible (scaleSizeSelector.get());
    scaleSizeSelector->setName ("Scale Size");


    //[UserPreSize]
	offsetLabel.reset(new Label("offsetLabel", "Offset\n0"));
	offsetLabel->setJustificationType(Justification::centred);
	offsetLabel->setColour(Label::ColourIds::textColourId, Colours::white);
	addAndMakeVisible(offsetLabel.get());
	
	circle = dynamic_cast<GroupingCircle*>(circleComponent.get());

	periodSlider->showNameLabel();
	generatorSlider->showNameLabel();
	scaleSizeSelector->showNameLabel();

	// TODO: implement period factors properly, and get rid of below
	periodFactorSelector->setInterceptsMouseClicks(false, false);

    //[/UserPreSize]

    setSize (800, 800);


    //[Constructor] You can add your own custom stuff here..

	// Set up default values, then set up listening
	periodSlider->setRange(5, 400, true, false);
	periodSlider->setValue(scaleStructure.getPeriod());

	scaleStructure.resetToPeriod(12);
	scaleStructure.setGeneratorIndex(scaleStructure.getSuggestedGeneratorIndex());
	scaleStructure.setSizeIndex(scaleStructure.getSuggestedSizeIndex());
	scaleStructure.setGeneratorOffset(1);

	generatorSlider->setList(scaleStructure.getValidGenerators());
	generatorSlider->setIndex(scaleStructure.getGeneratorIndex());
	scaleSizeSelector->setList(scaleStructure.getScaleSizes());
	scaleSizeSelector->setIndex(scaleStructure.getScaleSizeIndex());

	circleOffset = &circle->getOffsetValue();
	*circleOffset = 1;
	circle->setOffsetLimit(scaleStructure.getScaleSize() - 1);
	offsetLabel->setText("Offset\n" + String((int)circleOffset->getValue()), dontSendNotification);

	periodSlider->addListener(this);
	generatorSlider->addListener(this);
	scaleSizeSelector->addListener(this);
	circleOffset->addListener(this);

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
    periodFactorSelector = nullptr;
    scaleSizeSelector = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
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
    generatorSlider->setBounds (proportionOfWidth (0.5013f) - (proportionOfWidth (0.2503f) / 2), proportionOfHeight (0.4372f), proportionOfWidth (0.2503f), proportionOfHeight (0.1494f));
    periodSlider->setBounds (proportionOfWidth (0.5041f) - (proportionOfWidth (0.2503f) / 2), proportionOfHeight (0.2769f), proportionOfWidth (0.2503f), proportionOfHeight (0.1494f));
    generatorValueLbl->setBounds (proportionOfWidth (0.3606f) - (103 / 2), proportionOfHeight (0.7049f), 103, 24);
    stepSizePatternLbl->setBounds (proportionOfWidth (0.6401f) - (96 / 2), proportionOfHeight (0.7049f), 96, 24);
    periodFactorSelector->setBounds (proportionOfWidth (0.7470f), proportionOfHeight (0.2769f), proportionOfWidth (0.1448f), proportionOfHeight (0.3643f));
    scaleSizeSelector->setBounds (proportionOfWidth (0.5013f) - (proportionOfWidth (0.1800f) / 2), proportionOfHeight (0.6266f), proportionOfWidth (0.1800f), proportionOfHeight (0.1111f));
    //[UserResized] Add your own custom resize handling here..

	// TODO: implement (probably ex-projucer) this so that the bounds don't have to be set twice
	periodSlider->setCentrePosition(circle->getIntPointFromCenter(circle->getInnerRadius() * 0.45f, 0));
	generatorSlider->setCentrePosition(circle->getIntPointFromCenter(circle->getInnerRadius() * 0.05f, float_Pi));
	scaleSizeSelector->setCentrePosition(circle->getIntPointFromCenter(circle->getInnerRadius() *  7.0f / 10.0f, float_Pi));

	generatorValueLbl->setCentrePosition(circle->getIntPointFromCenter(circle->getInnerRadius() * 2.0f / 3.0f, float_Pi * 11.0f / 8.0f));
	stepSizePatternLbl->setCentrePosition(circle->getIntPointFromCenter(circle->getInnerRadius() * 2.0f / 3.0f, float_Pi * 5.0f / 8.0f));
	
	offsetLabel->setFont(Font().withHeight(getHeight() / 48.0f));
	offsetLabel->setSize(offsetLabel->getFont().getStringWidth("Offset") * 2, offsetLabel->getFont().getHeight() * 3);
	offsetLabel->setCentrePosition(circle->getIntPointFromCenter(circle->getInnerRadius() * 7.0f / 8.0f, 0));

	offsetArrows.clear();
	GroupingCircle::addArcToPath(offsetArrows, circle->getInnerCircleBounds().reduced(circle->getInnerRadius() / 13.0f), float_Pi / 24, float_Pi / 12, true);
	offsetArrows.lineTo(circle->getFloatPointFromCenter(circle->getInnerRadius() * 13.0f / 14.0f, float_Pi / 14));
	GroupingCircle::addArcToPath(offsetArrows, circle->getInnerCircleBounds().reduced(circle->getInnerRadius() / 13.0f), -float_Pi / 24, -float_Pi / 12, true);
	offsetArrows.lineTo(circle->getFloatPointFromCenter(circle->getInnerRadius() * 13.0f / 14.0f, -float_Pi / 14));

    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void ScaleStructureComponent::selectorValueChanged(NumberSelector* selectorThatHasChanged)
{
	if (selectorThatHasChanged == periodSlider.get())
	{
		periodSelected = periodSlider->getValue();
		DBG("SSC: Period changed to: " + String(periodSelected));

		scaleStructure.resetToPeriod(periodSelected);
		circle->updatePeriod(periodSelected);

		generatorSlider->setList(scaleStructure.getValidGenerators());
		generatorSlider->setIndex(scaleStructure.getSuggestedGeneratorIndex());
	}

	else if (selectorThatHasChanged == generatorSlider.get())
	{
		generatorSelected = generatorSlider->getIndex();
		scaleStructure.setGeneratorIndex(generatorSelected);
		circle->updateGenerator();
		DBG("SSC: Generator changed to: " + String(generatorSlider->getValue()));

		float cents = roundf(log2(pow(2, (double)generatorSlider->getValue() / periodSelected)) * 1200000) / 1000.0f;
		generatorValueLbl->setText(String(cents) + " cents", dontSendNotification);

		// update available sizes
		Array<int> sizes = scaleStructure.getScaleSizes();
		sizes.remove(0);
		scaleSizeSelector->setList(sizes);
		scaleSizeSelector->setIndex(scaleStructure.getSuggestedSizeIndex() - 1);
	}

	else if (selectorThatHasChanged == scaleSizeSelector.get())
	{
		scaleStructure.setSizeIndex(scaleSizeSelector->getIndex() + 1);
		DBG("SSC: Size changed to: " + String(scaleSizeSelector->getValue()));

		circle->setOffsetLimit(scaleSizeSelector->getValue() - 1);
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

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ScaleStructureComponent"
                 componentName="" parentClasses="public Component, public NumberSelector::Listener, public Value::Listener, public ChangeBroadcaster"
                 constructorParams="ScaleStructure&amp; scaleStructureIn, Array&lt;Colour&gt;&amp; colourTableIn"
                 variableInitialisers="scaleStructure(scaleStructureIn), colourTable(colourTableIn)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="800" initialHeight="800">
  <BACKGROUND backgroundColour="ff323e44"/>
  <GENERICCOMPONENT name="circleComponent" id="ec9c5dc09c2f91cf" memberName="circleComponent"
                    virtualName="" explicitFocusOrder="0" pos="0 0 100% 100%" class="GroupingCircle"
                    params="scaleStructure.getGeneratorChainReference(), scaleStructure.getGroupingSizesReference(), colourTable"/>
  <GENERICCOMPONENT name="Generator" id="efbe5586805bc62b" memberName="generatorSlider"
                    virtualName="NumberSelector" explicitFocusOrder="0" pos="50.135%c 43.716% 25.034% 14.936%"
                    class="Component" params="&quot;Generator&quot;, NumberSelector::SelectionType::List"/>
  <GENERICCOMPONENT name="Period" id="39f9599ebb9952a" memberName="periodSlider"
                    virtualName="NumberSelector" explicitFocusOrder="0" pos="50.406%c 27.687% 25.034% 14.936%"
                    class="Component" params="&quot;Period&quot;"/>
  <LABEL name="generatorValueLbl" id="7250d3d0fa11afcf" memberName="generatorValueLbl"
         virtualName="" explicitFocusOrder="0" pos="36.062%c 70.492% 103 24"
         edTextCol="ff000000" edBkgCol="0" labelText="700 cents" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="stepSizePatternLbl" id="b4e52c793121b24" memberName="stepSizePatternLbl"
         virtualName="" explicitFocusOrder="0" pos="64.005%c 70.492% 96 24"
         edTextCol="ff000000" edBkgCol="0" labelText="LLsLLLs&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <GENERICCOMPONENT name="Period Factor" id="a3462f3523b591da" memberName="periodFactorSelector"
                    virtualName="" explicitFocusOrder="0" pos="74.696% 27.687% 14.479% 36.43%"
                    class="NumberSelector" params="&quot;Period\nFactor&quot;, NumberSelector::SelectionType::List, NumberSelector::SelectorStyle::TickBox, NumberSelector::Orientation::Vertical"/>
  <GENERICCOMPONENT name="Scale Size" id="caf76440221c94" memberName="scaleSizeSelector"
                    virtualName="" explicitFocusOrder="0" pos="50.135%c 62.659% 17.997% 11.111%"
                    class="NumberSelector" params="&quot;Scale Size&quot;, NumberSelector::SelectionType::List"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

