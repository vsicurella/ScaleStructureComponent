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
GroupingCircle::GroupingCircle(const Array<int>& generatorChainIn, const Array<int>& degreeGroupSizesIn, Array<Colour>& groupColoursIn)
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
	g.drawEllipse(degreeInnerCircleBounds, 4.0f);*/

	strokeType.setStrokeThickness(2.0f);

	if (controlModeSelected == ControlMode::Notes)
	{
	}
	else
	{
	}

	g.strokePath(arcGroupSections, strokeType);
	g.strokePath(arcDegreeSections, strokeType);

	if (controlModeSelected == ControlMode::Layout)
	{
		// TODO: Outline degree group edges
	}
}

void GroupingCircle::resized()
{
	center = Point<float>(getWidth() / 2.0f, getHeight() / 2.0f);

	groupOuterRadius = getWidth() * borderRatio / 2.0f;
	degreeOuterRadius = groupOuterRadius * degreeToGroupRatio;
	degreeInnerRadius = degreeOuterRadius * degreeOuterToInnerRatio;

	groupRingWidth = groupOuterRadius - degreeOuterRadius;
	degreeRingWidth = degreeOuterRadius - degreeInnerRadius;

	groupMiddleRadius = (degreeOuterRadius + groupOuterRadius) / 2.0f;
	degreeMiddleRadius = (degreeInnerRadius + degreeOuterRadius) / 2.0f;

	groupOuterCircleBounds = getBounds().toFloat().reduced(proportionOfWidth(1 - borderRatio));
	groupInnerCircleBounds = groupOuterCircleBounds.reduced(groupRingWidth);
	degreeInnerCircleBounds = groupInnerCircleBounds.reduced(degreeRingWidth);
	
	angleIncrement = 2 * double_Pi / generatorChain.size();
	angleHalf = angleIncrement / 2.0f; // Middle of top section should be at 12 o'clock

	// determine circle offset, based off of middle angle of first degree group
	//circleOffset = (float_Pi * 1.5f) - (degreeGroupSizes[0] * angleIncrement / 2.0f);

	arcGroupSections.clear();
	int groupIndex = 0;
	int groupDegreesPassed = 0;
	float groupArcFrom = -angleHalf;// -circleOffset;

	float sizeAdj = 0.75f;
	float degreeLabelSize = degreeRingWidth * sizeAdj;
	float groupLabelSize = groupRingWidth * sizeAdj;

	Label* degreeLabel, *groupLabel;

	arcDegreeSections.clear();
	for (int i = 0; i < generatorChainLabels.size(); i++)
	{
		float ang = angleIncrement * i;
		float arcFrom = ang - angleHalf;
		float arcTo = ang + angleIncrement - angleHalf;
	
		// draw arc sections
		addArcToPath(arcDegreeSections, degreeInnerCircleBounds, arcFrom, arcTo, true);
		addArcToPath(arcDegreeSections, groupInnerCircleBounds, arcTo, arcFrom);
		arcDegreeSections.closeSubPath();

		// place labels
		// TODO: get section offset
		degreeLabel = generatorChainLabels[i];
		degreeLabel->setFont(Font().withHeight(degreeLabelSize));
		float degreeLabelWidth = degreeLabel->getFont().getStringWidthFloat(degreeLabel->getText());
		degreeLabel->setSize(jmax(degreeLabelWidth, degreeLabelSize), degreeLabelSize);

		degreeLabel->setCentrePosition(Point<int>(
			round(center.x + cosf(ang - circleOffset) * degreeMiddleRadius),
			round(center.y + sinf(ang - circleOffset) * degreeMiddleRadius)
		));

		int groupSize = degreeGroupSizes[groupIndex];
		if ((i + 1) - groupDegreesPassed >= groupSize && groupSize > 0)
		{
			addArcToPath(arcGroupSections, groupInnerCircleBounds, groupArcFrom, arcTo, true);
			addArcToPath(arcGroupSections, groupOuterCircleBounds, arcTo, groupArcFrom);
			arcGroupSections.closeSubPath();

			float groupCenterAngle = (groupArcFrom + arcTo) / 2.0f - angleHalf;

			groupLabel = degreeGroupLabels[groupIndex];
			groupLabel->setFont(Font().withHeight(groupLabelSize));
			float groupLabelWidth = groupLabel->getFont().getStringWidthFloat(groupLabel->getText());
			groupLabel->setSize(jmax(groupLabelWidth, groupLabelSize), groupLabelSize);

			groupLabel->setCentrePosition(Point<int>(
				round(center.x + cosf(groupCenterAngle - circleOffset ) * groupMiddleRadius),
				round(center.y + sinf(groupCenterAngle - circleOffset ) * groupMiddleRadius)
			));

			// TODO: place group degree labels
			DBG("Circle: Ended group " + String(groupIndex) + " size " + String(groupSize));
			groupDegreesPassed += groupSize;
			groupIndex++;
			groupSize = degreeGroupSizes[groupIndex];

			groupArcFrom = arcTo;
		}
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
		//generatorChainLabels[i]->setColour(Label::ColourIds::outlineColourId, Colours::white);
	}

	degreeGroupLabels.clear();
	for (auto groupSize : degreeGroupSizes)
	{
		Label* l = degreeGroupLabels.add(new Label());
		l->setJustificationType(Justification::centred);
		l->setText(String(groupSize), dontSendNotification);
		//l->setColour(Label::ColourIds::outlineColourId, Colours::white);
		addAndMakeVisible(l);
	}

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
