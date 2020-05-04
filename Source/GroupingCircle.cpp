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

	strokeType.setStrokeThickness(2.0f);

	if (controlModeSelected == ControlMode::Notes)
	{
	}
	else
	{
	}

	Colour c;
	int degIndex = 0;
	for (int i = 0; i < groupArcPaths.size(); i++)
	{
		// Draw groups
		Path& groupPath = groupArcPaths.getReference(i);
		c = groupColours[i];

		if (c.isTransparent())
			c = Colours::lightgrey;

		g.setColour(c);
		g.fillPath(groupPath);

		g.setColour(Colours::black);
		g.strokePath(groupPath, strokeType);

		groupSizeLabels[i]->setColour(Label::ColourIds::textColourId, c.contrasting(2.0f / 3.0f));

		// Draw degrees
		for (int d = 0; d < degreeGroupSizes[i]; d++)
		{
			Path& degreePath = degreeArcPaths.getReference(degIndex);
			g.setColour(c.brighter());
			g.fillPath(degreePath);

			g.setColour(c.darker());
			g.strokePath(degreePath, strokeType);

			degreeLabels[degIndex]->setColour(Label::ColourIds::textColourId, c.contrasting(2.0f / 3.0f));
			degIndex++;
		}
	}

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
	angleHalf = angleIncrement / 2.0f;

	// determine circle offset, based off of middle angle of first degree group
	circleOffset = degreeGroupSizes[0] * angleIncrement / 2.0f;

	int groupIndex = 0;
	int groupDegreesPassed = 0;
	
	float sizeAdj = 0.75f;
	float degreeLabelSize = degreeRingWidth * sizeAdj;
	float groupLabelSize = groupRingWidth * sizeAdj;

	float angle, angleTo, groupAngleFrom = -circleOffset; 
	float degLabelAngle, groupLabelAngle;

	Path degreePath, groupPath;
	Label* degreeLabel, *groupLabel;
	float degreeLabelWidth, groupLabelWidth;

	groupArcPaths.clear();
	degreeArcPaths.clear();
	for (int i = 0; i < degreeLabels.size(); i++)
	{
		angle = angleIncrement * i - circleOffset;
		angleTo = angle + angleIncrement;
	
		// draw arc sections
		degreePath = Path();
		addArcToPath(degreePath, degreeInnerCircleBounds, angle, angleTo, true);
		addArcToPath(degreePath, groupInnerCircleBounds, angleTo, angle);
		degreePath.closeSubPath();
		degreeArcPaths.add(degreePath);

		// place labels
		degreeLabel = degreeLabels[i];
		degreeLabel->setFont(Font().withHeight(degreeLabelSize));
		degreeLabelWidth = degreeLabel->getFont().getStringWidthFloat(degreeLabel->getText());
		degreeLabel->setSize(jmax(degreeLabelWidth, degreeLabelSize), degreeLabelSize);

		degLabelAngle =  angleTo - angleHalf - float_Pi / 2.0f;

		degreeLabel->setCentrePosition(Point<int>(
			round(center.x + cosf(degLabelAngle) * degreeMiddleRadius),
			round(center.y + sinf(degLabelAngle) * degreeMiddleRadius)
		));

		int groupSize = degreeGroupSizes[groupIndex];
		if ((i + 1) - groupDegreesPassed >= groupSize && groupSize > 0)
		{
			groupPath = Path();
			addArcToPath(groupPath, groupInnerCircleBounds, groupAngleFrom, angleTo, true);
			addArcToPath(groupPath, groupOuterCircleBounds, angleTo, groupAngleFrom);
			groupPath.closeSubPath();
			groupArcPaths.add(groupPath);

			groupLabelAngle = (groupAngleFrom + angleTo) / 2.0f - float_Pi / 2.0f;

			groupLabel = groupSizeLabels[groupIndex];
			groupLabel->setFont(Font().withHeight(groupLabelSize));
			groupLabelWidth = groupLabel->getFont().getStringWidthFloat(groupLabel->getText());
			groupLabel->setSize(jmax(groupLabelWidth, groupLabelSize), groupLabelSize);

			groupLabel->setCentrePosition(Point<int>(
				round(center.x + cosf(groupLabelAngle) * groupMiddleRadius),
				round(center.y + sinf(groupLabelAngle) * groupMiddleRadius)
			));

			groupDegreesPassed += groupSize;
			groupIndex++;
			groupSize = degreeGroupSizes[groupIndex];

			groupAngleFrom = angleTo;
		}
	}

	DBG("Circle: Resized");
}

void GroupingCircle::updatePeriod(int periodIn)
{
	DBG("Circle: Updating");

	removeAllChildren();

	degreeLabels.clear();
	for (int i = 0; i < periodIn; i++)
	{
		Label* l = degreeLabels.add(new Label());
		l->setJustificationType(Justification::centred);
		addAndMakeVisible(l);
	}
}

void GroupingCircle::updateGenerator()
{
	DBG("Circle: Generator changed, updating labels");
	for (int i = 0; i < degreeLabels.size(); i++)
	{
		degreeLabels[i]->setText(String(generatorChain[i]), dontSendNotification);
		//degreeLabels[i]->setColour(Label::ColourIds::outlineColourId, Colours::white);
	}

	groupSizeLabels.clear();
	for (auto groupSize : degreeGroupSizes)
	{
		Label* l = groupSizeLabels.add(new Label());
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
