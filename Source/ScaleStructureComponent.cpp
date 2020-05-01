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
ScaleStructureComponent::ScaleStructureComponent (ScaleStructure& scaleStructureIn)
    : scaleStructure(scaleStructureIn)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    circleComponent.reset (new GroupingCircle (scaleStructure.getGeneratorChainReference(), scaleStructure.getGroupingSizesReference(), degreeGroupColours));
    addAndMakeVisible (circleComponent.get());
    circleComponent->setName ("circleComponent");

    offsetSlider.reset (new Component());
    addAndMakeVisible (offsetSlider.get());
    offsetSlider->setName ("offsetSlider");

    generatorSlider.reset (new NumberSelector());
    addAndMakeVisible (generatorSlider.get());
    generatorSlider->setName ("generatorSlider");

    periodSlider.reset (new NumberSelector());
    addAndMakeVisible (periodSlider.get());
    periodSlider->setName ("periodSlider");

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
	//generatorSlider->setSelectionType(NumberSelector::SelectionType::List);
    //[/UserPreSize]

    setSize (800, 800);


    //[Constructor] You can add your own custom stuff here..
	circle = dynamic_cast<GroupingCircle*>(circleComponent.get());

	periodSlider->addListener(this);
	generatorSlider->addListener(this);

	periodSlider->setRange(5, 400, true, false);
	periodSlider->setValue(scaleStructure.getPeriod());
    //[/Constructor]
}

ScaleStructureComponent::~ScaleStructureComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    circleComponent = nullptr;
    offsetSlider = nullptr;
    generatorSlider = nullptr;
    periodSlider = nullptr;
    generatorValueLbl = nullptr;
    stepSizePatternLbl = nullptr;


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
    //[/UserPaint]
}

void ScaleStructureComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    circleComponent->setBounds (0, 0, proportionOfWidth (1.0000f), proportionOfHeight (1.0000f));
    offsetSlider->setBounds (proportionOfWidth (0.5004f) - (proportionOfWidth (0.1449f) / 2), proportionOfHeight (0.0604f), proportionOfWidth (0.1449f), proportionOfHeight (0.0638f));
    generatorSlider->setBounds (proportionOfWidth (0.5000f) - (proportionOfWidth (0.1742f) / 2), proportionOfHeight (0.5200f), proportionOfWidth (0.1742f), proportionOfHeight (0.1378f));
    periodSlider->setBounds (proportionOfWidth (0.5000f) - (proportionOfWidth (0.1742f) / 2), proportionOfHeight (0.3400f), proportionOfWidth (0.1742f), proportionOfHeight (0.1378f));
    generatorValueLbl->setBounds (proportionOfWidth (0.3600f) - (103 / 2), proportionOfHeight (0.7500f), 103, 24);
    stepSizePatternLbl->setBounds (proportionOfWidth (0.6396f) - (96 / 2), proportionOfHeight (0.7500f), 96, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void ScaleStructureComponent::selectorValueChanged(NumberSelector* selectorThatHasChanged)
{
	if (selectorThatHasChanged == periodSlider.get())
	{
		periodSelected = periodSlider->getValue();
		DBG("SSC: Period changed to " + String(periodSelected));

		scaleStructure.resetToPeriod(periodSelected);
		circle->updatePeriod(periodSelected);

		generatorSlider->setRange(1, periodSelected, false, false);
		generatorSlider->setValue(scaleStructure.getSuggestedGenerator());

		//generatorSlider->setList(getCoprimes(periodSlider->getValue()));
		//generatorSlider->setIndex(generatorSlider->getList().size() / 2);
	}

	else if (selectorThatHasChanged == generatorSlider.get())
	{
		generatorSelected = generatorSlider->getValue();
		scaleStructure.setGenerator(generatorSelected);
		circle->updateGenerator();
		DBG("SSC: Generator changed to : " + String(generatorSelected));

		float cents = roundf(log2(pow(2, (double)generatorSelected / periodSelected)) * 1200000) / 1000.0f;
		generatorValueLbl->setText(String(cents) + " cents", dontSendNotification);
		return;
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
                 componentName="" parentClasses="public Component, public NumberSelector::Listener"
                 constructorParams="ScaleStructure&amp; scaleStructureIn" variableInitialisers="scaleStructure(scaleStructureIn)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="800" initialHeight="800">
  <BACKGROUND backgroundColour="ff323e44"/>
  <GENERICCOMPONENT name="circleComponent" id="ec9c5dc09c2f91cf" memberName="circleComponent"
                    virtualName="" explicitFocusOrder="0" pos="0 0 100% 100%" class="GroupingCircle"
                    params="scaleStructure.getGeneratorChainReference(), scaleStructure.getGroupingSizesReference(), degreeGroupColours"/>
  <GENERICCOMPONENT name="offsetSlider" id="1bfdf4c1ccc67e63" memberName="offsetSlider"
                    virtualName="" explicitFocusOrder="0" pos="50.038%c 6.036% 14.489% 6.378%"
                    class="Component" params=""/>
  <GENERICCOMPONENT name="generatorSlider" id="efbe5586805bc62b" memberName="generatorSlider"
                    virtualName="NumberSelector" explicitFocusOrder="0" pos="50%c 52.05% 17.417% 13.781%"
                    class="Component" params=""/>
  <GENERICCOMPONENT name="periodSlider" id="39f9599ebb9952a" memberName="periodSlider"
                    virtualName="NumberSelector" explicitFocusOrder="0" pos="50%c 34.055% 17.417% 13.781%"
                    class="Component" params=""/>
  <LABEL name="generatorValueLbl" id="7250d3d0fa11afcf" memberName="generatorValueLbl"
         virtualName="" explicitFocusOrder="0" pos="35.998%c 74.943% 103 24"
         edTextCol="ff000000" edBkgCol="0" labelText="700 cents" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="stepSizePatternLbl" id="b4e52c793121b24" memberName="stepSizePatternLbl"
         virtualName="" explicitFocusOrder="0" pos="63.964%c 74.943% 96 24"
         edTextCol="ff000000" edBkgCol="0" labelText="LLsLLLs&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

