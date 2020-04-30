/*
  ==============================================================================

    GroupingCircle.cpp
    Created: 26 Apr 2020 4:59:12pm
    Author:  Vincenzo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "GroupingCircle.h"

//==============================================================================
GroupingCircle::GroupingCircle(const Array<int>& generatorChainIn, const Array<int> degreeGroupSizesIn, Array<Colour>& groupColoursIn)
	:	generatorChain(generatorChainIn),
		degreeGroupSizes(degreeGroupSizesIn),
		groupColours(groupColoursIn)
{
	//jassert(degreeGroupSize.size() == groupColours.size())
	//updateChildren();
	
}

GroupingCircle::~GroupingCircle()
{
}

GroupingCircle::ControlMode GroupingCircle::getControlMode()
{
	return controlModeSelected;
}

void GroupingCircle::setControlMode(ControlMode controlModeIn)
{
	controlModeSelected = controlModeIn;
}

void GroupingCircle::paint (Graphics& g)
{
	g.fillAll(Colour());

	const float innerToOuterRadius = 3.0f / 4.0f;
	const float ringWidth = proportionOfWidth(1 - innerToOuterRadius) / 2.0f;
	const float circleRadiusOuter = getWidth() / 2.0f;
	const float circleRadiusInner = circleRadiusOuter * innerToOuterRadius;
	const Point<float> center = Point<float>(getWidth() / 2.0f, getHeight() / 2.0f);

	const Rectangle<float> innerCircleBounds = getBounds().toFloat().reduced(ringWidth);

	// Draw outline
	g.setColour(Colours::black);
	g.drawEllipse(getBounds().toFloat(), 4.0f);
	g.drawEllipse(innerCircleBounds, 4.0f);

	// Set up degree edges
	Array<Line<float>> radiLines;

	for (int i = 0; i < generatorChain.size(); i++)
	{
		float ang = 2 * float_Pi * i / generatorChain.size();
		
		float lineStartX = center.x + cosf(ang) * circleRadiusInner;
		float lineStartY = center.y + sinf(ang) * circleRadiusInner;

		float lineEndX = center.x + cosf(ang) * circleRadiusOuter;
		float lineEndY = center.y + sinf(ang) * circleRadiusOuter;
		
		radiLines.add(Line<float>(lineStartX, lineStartY, lineEndX, lineEndY));
		DBG("CIRCLE:\tAngle #" + String(i) + " = " + String(ang) + " for degree " + String(generatorChain[i]));
	}

	// TODO: Use Path based method to be able to rotate lines

	if (showDegreesInLayoutMode)
	{
		if (controlModeSelected == ControlMode::Notes)
		{
			// TODO: Set up different styles
		}
		else
		{

		}

		int i = 0;
		for (auto line : radiLines)
		{
			g.drawLine(line, 2.0f);
		}

	}

	if (controlModeSelected == ControlMode::Layout)
	{
		// TODO: Outline degree group edges
	}
}

void GroupingCircle::resized()
{


}

void GroupingCircle::updateChildren()
{
	removeAllChildren();

	generatorChainLabels.clear();
	for (auto degree : generatorChain)
	{
		Label* l = generatorChainLabels.add(new Label());
		l->setText(String(degree), dontSendNotification);
		addAndMakeVisible(l);
	}

	degreeGroupLabels.clear();
	for (auto groupSize : degreeGroupSizes)
	{
		Label* l = degreeGroupLabels.add(new Label());
		l->setText(String(groupSize), dontSendNotification);
		addAndMakeVisible(l);
	}
}