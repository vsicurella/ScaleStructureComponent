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
#include "TransparentDropDown.h"

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
                                 public ChangeBroadcaster,
                                 public ComboBox::Listener,
                                 public Button::Listener
{
public:
    //==============================================================================
    ScaleStructureComponent (ScaleStructure& scaleStructureIn, Array<Colour>& colourTableIn);
    ~ScaleStructureComponent() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void paintOverChildren(Graphics& g) override;

	void comboBoxChanged(ComboBox* comboBoxThatChanged) override;
	void buttonClicked(Button* buttonThatWasClicked) override;
	void selectorValueChanged(NumberSelector* selectorThatHasChanged) override;
	void valueChanged(Value& valueThatHasChanged) override;

	void updateGenerators();
	void updateScaleSizes();

	void updatePeriodFactor(int factorIndexIn);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	// Functional Elements
	ScaleStructure& scaleStructure;
	Array<Colour>& colourTable;

	GroupingCircle* circle;
	Value* circleOffset;

	TooltipWindow tooltipWindow;

	// Components
	std::unique_ptr<Label> offsetLabel;
	Path offsetArrows;

	std::unique_ptr<TransparentDropDown> dropdownLookAndFeel;
	std::unique_ptr<ComboBox> sizeBox;

	Path periodFactorButtonShape;
	std::unique_ptr<ShapeButton> periodFactorButton;
	PopupMenu periodFactorMenu;
	

	// Helpers
	int periodSelected;
	int generatorSelected;

	Array<int> periodFactors;
	int periodFactorSelected = 0;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<GroupingCircle> circleComponent;
    std::unique_ptr<NumberSelector> generatorSlider;
    std::unique_ptr<NumberSelector> periodSlider;
    std::unique_ptr<Label> generatorValueLbl;
    std::unique_ptr<Label> stepSizePatternLbl;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScaleStructureComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

