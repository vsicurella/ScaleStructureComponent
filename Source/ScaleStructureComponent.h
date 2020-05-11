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

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>

#include "NumberSelector.h"
#include "GroupingCircle.h"
#include "ScaleStructure.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ScaleStructureComponent  : public Component,
                                 public NumberSelector::Listener,
                                 public Value::Listener,
                                 public ChangeBroadcaster
{
public:
    //==============================================================================
    ScaleStructureComponent (ScaleStructure& scaleStructureIn, Array<Colour>& colourTableIn);
    ~ScaleStructureComponent() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void selectorValueChanged(NumberSelector* selectorThatHasChanged) override;
	void valueChanged(Value& valueThatHasChanged) override;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	ScaleStructure& scaleStructure;
	Array<Colour>& colourTable;

	GroupingCircle* circle;
	Value* circleOffset;

	std::unique_ptr<Label> offsetLabel;

	int periodSelected;
	int generatorSelected;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<GroupingCircle> circleComponent;
    std::unique_ptr<NumberSelector> generatorSlider;
    std::unique_ptr<NumberSelector> periodSlider;
    std::unique_ptr<Label> generatorValueLbl;
    std::unique_ptr<Label> stepSizePatternLbl;
    std::unique_ptr<NumberSelector> periodFactorSelector;
    std::unique_ptr<NumberSelector> scaleSizeSelector;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScaleStructureComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

