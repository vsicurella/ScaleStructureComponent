/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ScaleStructureComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    
	ScaleStructure scaleStructure;
    std::unique_ptr<ScaleStructureComponent> scaleController;

	// Test colours
	Array<Colour> colourTable = 
	{
		Colours::lightyellow,
		Colours::green,
		Colours::turquoise,
		Colours::yellow,
		Colours::mediumpurple,
		Colours::orange
	};


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
