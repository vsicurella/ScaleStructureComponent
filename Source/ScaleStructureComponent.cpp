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

    offsetSlider.reset (new NumberSelector ("Offset"));
    addAndMakeVisible (offsetSlider.get());
    offsetSlider->setName ("Offset");

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
	periodSlider->showNameLabel();
	//periodFactorSelector->showNameLabel();
	generatorSlider->showNameLabel();
	offsetSlider->showNameLabel();
	scaleSizeSelector->showNameLabel();
    //[/UserPreSize]

    setSize (800, 800);


    //[Constructor] You can add your own custom stuff here..
	circle = dynamic_cast<GroupingCircle*>(circleComponent.get());

	periodSlider->addListener(this);
	generatorSlider->addListener(this);
	offsetSlider->addListener(this);
	scaleSizeSelector->addListener(this);

	periodSlider->setRange(5, 400, true, false);
	periodSlider->setValue(scaleStructure.getPeriod());

	offsetSlider->setValue(1);
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
    //[/UserPaint]
}

void ScaleStructureComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    circleComponent->setBounds (0, 0, proportionOfWidth (1.0000f), proportionOfHeight (1.0000f));
    offsetSlider->setBounds (proportionOfWidth (0.5015f) - (proportionOfWidth (0.1802f) / 2), proportionOfHeight (0.1913f), proportionOfWidth (0.1802f), proportionOfHeight (0.1105f));
    generatorSlider->setBounds (proportionOfWidth (0.5000f) - (proportionOfWidth (0.1802f) / 2), proportionOfHeight (0.4613f), proportionOfWidth (0.1802f), proportionOfHeight (0.1105f));
    periodSlider->setBounds (proportionOfWidth (0.5000f) - (proportionOfWidth (0.1800f) / 2), proportionOfHeight (0.3155f), proportionOfWidth (0.1800f), proportionOfHeight (0.1100f));
    generatorValueLbl->setBounds (proportionOfWidth (0.3600f) - (103 / 2), proportionOfHeight (0.7050f), 103, 24);
    stepSizePatternLbl->setBounds (proportionOfWidth (0.6396f) - (96 / 2), proportionOfHeight (0.7050f), 96, 24);
    periodFactorSelector->setBounds (proportionOfWidth (0.6306f), proportionOfHeight (0.1913f), proportionOfWidth (0.1441f), proportionOfHeight (0.3645f));
    scaleSizeSelector->setBounds (proportionOfWidth (0.5000f) - (proportionOfWidth (0.1802f) / 2), proportionOfHeight (0.6048f), proportionOfWidth (0.1802f), proportionOfHeight (0.1105f));
    //[UserResized] Add your own custom resize handling here..
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
		//circle->updateGenerator();
		DBG("SSC: Generator changed to: " + String(generatorSlider->getValue()));

		float cents = roundf(log2(pow(2, (double)generatorSlider->getValue() / periodSelected)) * 1200000) / 1000.0f;
		generatorValueLbl->setText(String(cents) + " cents", dontSendNotification);

		// update available sizes
		Array<int> sizes = scaleStructure.getScaleSizes();
		sizes.remove(0);
		scaleSizeSelector->setList(sizes);
		scaleSizeSelector->setIndex(scaleStructure.getSuggestedSizeIndex() - 1);
	}

	else if (selectorThatHasChanged == offsetSlider.get())
	{
		scaleStructure.setGeneratorOffset(offsetSlider->getValue());
		circle->updateGenerator();
		DBG("SSC: Generator Offset changed to: " + String(offsetSlider->getValue()));
		stepSizePatternLbl->setText(scaleStructure.getLsSteps(), dontSendNotification);
	}

	else if (selectorThatHasChanged == scaleSizeSelector.get())
	{
		scaleStructure.setSizeIndex(scaleSizeSelector->getIndex() + 1);
		DBG("SSC: Size changed to: " + String(scaleSizeSelector->getValue()));

		offsetSlider->setRange(0, scaleSizeSelector->getValue());

		stepSizePatternLbl->setText(scaleStructure.getLsSteps(), dontSendNotification);
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
  <GENERICCOMPONENT name="Offset" id="1bfdf4c1ccc67e63" memberName="offsetSlider"
                    virtualName="" explicitFocusOrder="0" pos="50.15%c 19.134% 18.018% 11.048%"
                    class="NumberSelector" params="&quot;Offset&quot;"/>
  <GENERICCOMPONENT name="Generator" id="efbe5586805bc62b" memberName="generatorSlider"
                    virtualName="NumberSelector" explicitFocusOrder="0" pos="50%c 46.128% 18.018% 11.048%"
                    class="Component" params="&quot;Generator&quot;, NumberSelector::SelectionType::List"/>
  <GENERICCOMPONENT name="Period" id="39f9599ebb9952a" memberName="periodSlider"
                    virtualName="NumberSelector" explicitFocusOrder="0" pos="50%c 31.549% 18.018% 11.048%"
                    class="Component" params="&quot;Period&quot;"/>
  <LABEL name="generatorValueLbl" id="7250d3d0fa11afcf" memberName="generatorValueLbl"
         virtualName="" explicitFocusOrder="0" pos="35.998%c 70.501% 103 24"
         edTextCol="ff000000" edBkgCol="0" labelText="700 cents" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="stepSizePatternLbl" id="b4e52c793121b24" memberName="stepSizePatternLbl"
         virtualName="" explicitFocusOrder="0" pos="63.964%c 70.501% 96 24"
         edTextCol="ff000000" edBkgCol="0" labelText="LLsLLLs&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <GENERICCOMPONENT name="Period Factor" id="a3462f3523b591da" memberName="periodFactorSelector"
                    virtualName="" explicitFocusOrder="0" pos="63.063% 19.134% 14.414% 36.446%"
                    class="NumberSelector" params="&quot;Period\nFactor&quot;, NumberSelector::SelectionType::List, NumberSelector::SelectorStyle::TickBox, NumberSelector::Orientation::Vertical"/>
  <GENERICCOMPONENT name="Scale Size" id="caf76440221c94" memberName="scaleSizeSelector"
                    virtualName="" explicitFocusOrder="0" pos="50%c 60.478% 18.018% 11.048%"
                    class="NumberSelector" params="&quot;Scale Size&quot;, NumberSelector::SelectionType::List"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

