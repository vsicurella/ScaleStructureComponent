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

	// Draw outline
	g.setColour(Colours::black);
	/*g.drawEllipse(getBounds().toFloat(), 4.0f);
	g.drawEllipse(innerCircleBounds, 4.0f);*/


	if (showDegreesInLayoutMode)
	{
		strokeType.setStrokeThickness(2.0f);

		if (controlModeSelected == ControlMode::Notes)
		{
		}
		else
		{
		}

		// Draw degree edges
		//int i = 0;
		//for (auto line : radiLines)
		//{
		//	g.drawLine(line, 2.0f);
		//}

		g.strokePath(arcDegreeSections, strokeType);
	}

	if (controlModeSelected == ControlMode::Layout)
	{
		// TODO: Outline degree group edges
	}
}

void GroupingCircle::resized()
{
	circleRadiusOuter = getWidth() / 2.0f;
	circleRadiusInner = circleRadiusOuter * innerToOuterRadius;
	circleRadiusMiddle = (circleRadiusInner + circleRadiusOuter) / 2.0f;

	center = Point<float>(getWidth() / 2.0f, getHeight() / 2.0f);
	ringWidth = proportionOfWidth(1 - innerToOuterRadius) / 2.0f;
	innerCircleBounds = getBounds().toFloat().reduced(ringWidth);
	
	angleIncrement = 2 * double_Pi / generatorChain.size();
	angleHalf = angleIncrement / 2.0f; // Middle of top section should be at 12 o'clock

	arcDegreeSections.clear();
	for (int i = 0; i < generatorChainLabels.size(); i++)
	{
		float ang = angleIncrement * i;
		float arcFrom = ang - angleHalf;
		float arcTo = ang + angleIncrement - angleHalf;

		// draw arc sections
		addArcToPath(arcDegreeSections, innerCircleBounds, arcFrom, arcTo, true);
		addArcToPath(arcDegreeSections, getBounds().toFloat(), arcTo, arcFrom);
		arcDegreeSections.closeSubPath();

		// place labels
		// TODO: get section offset
		generatorChainLabels[i]->setSize(ringWidth, ringWidth);
		generatorChainLabels[i]->setFont(Font().withHeight(ringWidth / 4.0f));
		generatorChainLabels[i]->setCentrePosition(Point<int>(
			center.x + cosf(ang - float_Pi / 2.0f) * circleRadiusMiddle,
			center.y + sinf(ang - float_Pi / 2.0f) * circleRadiusMiddle
		));
	}

	DBG("Circle: Resized");
}

void GroupingCircle::updatePeriod(int periodIn)
{
	DBG("Circle: Updating");

	removeAllChildren();

	generatorChainLabels.clear();
	for (int i = 0; i < periodIn; i++)
	{
		Label* l = generatorChainLabels.add(new Label());
		l->setJustificationType(Justification::centred);
		addAndMakeVisible(l);
	}
}

void GroupingCircle::updateGenerator()
{
	DBG("Circle: Generator changed, updating labels");
	for (int i = 0; i < generatorChainLabels.size(); i++)
	{
		generatorChainLabels[i]->setText(String(generatorChain[i]), dontSendNotification);
	}

	//degreeGroupLabels.clear();
	//for (auto groupSize : degreeGroupSizes)
	//{
	//	Label* l = degreeGroupLabels.add(new Label());
	//	l->setText(String(groupSize), dontSendNotification);
	//	addAndMakeVisible(l);
	//}

	resized();
	repaint();
}

void GroupingCircle::addArcToPath(Path& pathIn, Rectangle<float>& ellipseBounds, float fromRadians, float toRadians, bool startAsNewSubPath)
{
	pathIn.addArc(
		ellipseBounds.getX(),
		ellipseBounds.getY(),
		ellipseBounds.getWidth(),
		ellipseBounds.getHeight(),
		fromRadians,
		toRadians,
		startAsNewSubPath
	);
}
