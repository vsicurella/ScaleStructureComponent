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
GroupingCircle::GroupingCircle(const Array<Array<int>>& degreeGroupingsIn, Array<Colour>& groupColoursIn)
	:	degreeGroupings(degreeGroupingsIn),
		colourTable(groupColoursIn)
{

}

GroupingCircle::~GroupingCircle()
{
}

float GroupingCircle::getInnerRadius() const
{
	return degreeInnerRadius;
}

float GroupingCircle::getMiddleRadius() const
{
	return degreeOuterRadius;
}

float GroupingCircle::getOuterRadius() const
{
	return groupOuterRadius;
}

Rectangle<float> GroupingCircle::getInnerCircleBounds() const
{
	return degreeInnerCircleBounds;
}

Rectangle<float> GroupingCircle::getMiddleCircleBounds() const
{
	return groupInnerCircleBounds;
}

Rectangle<float> GroupingCircle::getOuterCircleBounds() const
{
	return groupOuterCircleBounds;
}

Point<float> GroupingCircle::getCenter() const
{
	return center;
}

Point<float> GroupingCircle::getFloatPointFromCenter(float radius, float angle) const
{
	return center.getPointOnCircumference(radius, angle);

}

Point<int> GroupingCircle::getIntPointFromCenter(float radius, float angle) const
{
	return getFloatPointFromCenter(radius, angle).toInt();
}

GroupingCircle::ControlMode GroupingCircle::getControlMode()
{
	return controlModeSelected;
}

Value& GroupingCircle::getOffsetValue()
{
	return generatorOffset;
}

void GroupingCircle::setControlMode(ControlMode controlModeIn)
{
	controlModeSelected = controlModeIn;
}

void GroupingCircle::setGeneratorOffset(int offsetIn, bool upgradeDegreeLabels)
{
	generatorOffset.setValue(offsetIn);

	if (upgradeDegreeLabels)
		updateGenerator();
}

void GroupingCircle::setOffsetLimit(int offsetLimitIn)
{
	offsetLimit = offsetLimitIn;
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

	Colour groupColour;
	Colour degreeColour;
	int degIndex = 0;
	for (int i = 0; i < groupArcPaths.size(); i++)
	{
		// Draw groups
		Path& groupPath = groupArcPaths.getReference(i);
		groupColour = colourTable[i];

		if (groupColour.isTransparent())
			groupColour = Colours::lightgrey;

		if (groupSectorMouseOver[i])
			groupColour = groupColour.contrasting(highlightContrastRatio);

		g.setColour(groupColour);
		g.fillPath(groupPath);

		g.setColour(Colours::black);
		g.strokePath(groupPath, strokeType);

		groupSizeLabels[i]->setColour(Label::ColourIds::textColourId, groupColour.contrasting(labelContrastRatio));

		// Draw degrees
		for (int d = 0; d < groupSizes[i]; d++)
		{
			degreeColour = groupColour;

			if (degreeSectorMouseOver[degIndex])
				degreeColour = groupColour.contrasting(highlightContrastRatio);

			Path& degreePath = degreeArcPaths.getReference(degIndex);
			
			g.setColour(degreeColour.brighter());
			g.fillPath(degreePath);

			g.setColour(degreeColour.darker());
			g.strokePath(degreePath, strokeType);

			degreeLabels[degIndex]->setColour(Label::ColourIds::textColourId, degreeColour.contrasting(labelContrastRatio));
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
	circleOffset = groupSizes[0] * angleIncrement / 2.0f;

	int groupIndex = 0;
	int groupDegreesPassed = 0;
	
	float degreeLabelSize = jmin(degreeRingWidth, 2 * float_Pi * degreeMiddleRadius / degreeLabels.size() ) * sectorLabelSizeRatio;
	float groupLabelSize = groupRingWidth * sectorLabelSizeRatio;

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
		degreeLabelWidth = degreeLabel->getFont().getStringWidthFloat(" " + degreeLabel->getText() + " ");
		degreeLabel->setSize(jmax(degreeLabelWidth, degreeLabelSize), degreeLabelSize);

		degLabelAngle =  angleTo - angleHalf - float_HalfPi;

		degreeLabel->setCentrePosition(Point<int>(
			round(center.x + cosf(degLabelAngle) * degreeMiddleRadius),
			round(center.y + sinf(degLabelAngle) * degreeMiddleRadius)
		));

		int groupSize = groupSizes[groupIndex];
		if ((i + 1) - groupDegreesPassed >= groupSize && groupSize > 0)
		{
			groupPath = Path();
			addArcToPath(groupPath, groupInnerCircleBounds, groupAngleFrom, angleTo, true);
			addArcToPath(groupPath, groupOuterCircleBounds, angleTo, groupAngleFrom);
			groupPath.closeSubPath();
			groupArcPaths.add(groupPath);

			groupLabelAngle = (groupAngleFrom + angleTo) / 2.0f - float_HalfPi;

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
			groupSize = groupSizes[groupIndex];

			groupAngleFrom = angleTo;
		}
	}

	DBG("Circle: Resized");
}

void GroupingCircle::mouseMove(const MouseEvent& event)
{
	float mouseRadius = event.position.getDistanceFrom(center);
	bool dirty = false;

	// If mouse is not in rings, remove highlights
	if (mouseRadius < degreeInnerRadius || mouseRadius > groupOuterRadius)
	{
		degreeSectorMouseOver.fill(false);
		groupSectorMouseOver.fill(false);
		dirty = true;
	}

	else
	{
		float angle = getNormalizedMouseAngle(event);
		int degreeIndex = mouseInDegreeSector(event, angle);

		// Check Degree Sectors
		if (mouseRadius < degreeOuterRadius)
		{
			if (lastDegreeSectorMouseIn != degreeIndex)
			{
				if (lastDegreeSectorMouseIn > -1)
				{
					degreeSectorMouseOver.set(lastDegreeSectorMouseIn, false);
				}

				degreeSectorMouseOver.set(degreeIndex, true);
				lastDegreeSectorMouseIn = degreeIndex;
				dirty = true;
			}

			if (lastGroupSectorMouseIn > -1)
			{
				groupSectorMouseOver.set(lastGroupSectorMouseIn, false);
				lastGroupSectorMouseIn = -1;
				dirty = true;
			}
		}

		// Check Group Sectors
		else if (mouseRadius < groupOuterRadius)
		{
			int groupIndex = mouseInGroupSector(degreeIndex);

			lastGroupSectorMouseIn = groupSectorMouseOver.indexOf(true);
			if (lastGroupSectorMouseIn != groupIndex)
			{
				if (lastGroupSectorMouseIn > -1)
				{
					groupSectorMouseOver.set(lastGroupSectorMouseIn, false);
				}

				groupSectorMouseOver.set(groupIndex, true);
				lastGroupSectorMouseIn = groupIndex;
				dirty = true;
			}

			if (lastDegreeSectorMouseIn > -1)
			{
				degreeSectorMouseOver.set(lastDegreeSectorMouseIn, false);
				lastDegreeSectorMouseIn = -1;
				dirty = true;
			}
		}
	}

	if (dirty)
		repaint();
}

void GroupingCircle::mouseDrag(const MouseEvent& event)
{
	float mouseDownRadius = event.mouseDownPosition.getDistanceFrom(center);
	bool dirty = false;

	if (mouseDownRadius >= degreeInnerRadius && mouseDownRadius < degreeOuterRadius)
	{
		float mouseRadius = event.position.getDistanceFrom(center);
		float angle = getNormalizedMouseAngle(event);
		int degreeIndex = mouseInDegreeSector(event, angle);

		if (lastDegreeSectorMouseIn != degreeIndex)
		{
			int offset = degreeIndex - lastDegreeSectorMouseIn + (int)generatorOffset.getValue();

			if (offset > -1 && offset <= offsetLimit)
			{
				generatorOffset.setValue(offset);
				DBG("Moved by " + String(degreeIndex - lastDegreeSectorMouseIn) + "\tNew offset: " + generatorOffset.getValue().toString());
				dirty = true;
			}

			degreeSectorMouseOver.set(lastDegreeSectorMouseIn, false);
			degreeSectorMouseOver.set(degreeIndex, true);
			lastDegreeSectorMouseIn = degreeIndex;
		}
	}

	if (dirty)
	{
		resized();
	}
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
		l->setInterceptsMouseClicks(false, false);
		addAndMakeVisible(l);
	}

	degreeSectorMouseOver.resize(periodIn);
	degreeSectorMouseOver.fill(false);
}

void GroupingCircle::updateGenerator()
{
	// Update generatorChain and groupSizes
	generatorChain.clear();
	groupSizes.clear();
	for (int g = 0; g < degreeGroupings.size(); g++)
	{
		const Array<int>& group = degreeGroupings.getReference(g);
		groupSizes.add(group.size());

		for (auto degree : group)
		{
			generatorChain.add(degree);
		}
	}
	
	for (int i = 0; i < degreeLabels.size(); i++)
	{
		degreeLabels[i]->setText(String(generatorChain[i]), dontSendNotification);
		//degreeLabels[i]->setColour(Label::ColourIds::outlineColourId, Colours::white);
	}

	groupSizeLabels.clear();
	for (int i = 0; i < groupSizes.size(); i++)
	{
		Label* l = groupSizeLabels.add(new Label());
		l->setJustificationType(Justification::centred);
		l->setText(String(groupSizes[i]), dontSendNotification);
		l->setInterceptsMouseClicks(false, false);
		//l->setColour(Label::ColourIds::outlineColourId, Colours::white);

		// First one will be replaced by scaleSizeSelector
		if (i > 0)
			addAndMakeVisible(l);
	}

	groupSectorMouseOver.resize(groupSizeLabels.size());
	groupSectorMouseOver.fill(false);

	resized();
	repaint();
}

float GroupingCircle::getNormalizedMouseAngle(const MouseEvent& event)
{
	float angle = atan2f(event.position.x - center.x, center.y - event.position.y) + (groupSizes[0] / 2.0f * angleIncrement);
	if (angle < 0) angle += float_Tau;
	if (angle >= float_Tau) angle -= float_Tau;

	return angle;
}

int GroupingCircle::mouseInDegreeSector(const MouseEvent& event, float angle)
{
	return (int)(angle / angleIncrement) % generatorChain.size();
}

int GroupingCircle::mouseInDegreeRingSector(const MouseEvent& event, float radiusFromCenter, float angle)
{
	if (radiusFromCenter >= degreeInnerRadius && radiusFromCenter < degreeOuterRadius)
		return mouseInDegreeSector(event, angle);

	return -1;
}

int GroupingCircle::mouseInGroupSector(int degreeIndex)
{
	int groupIndex = 0;
	int groupSize = groupSizes[groupIndex];
	while (degreeIndex >= groupSize)
	{
		degreeIndex -= groupSize;

		if (degreeIndex < 0 || groupIndex == groupSizes.size() - 1)
			return -1;

		groupSize = groupSizes[++groupIndex];
	}
	
	return groupIndex;
}

int GroupingCircle::mouseInGroupRingSector(const MouseEvent& event, float radiusFromCenter, float angle)
{
	if (radiusFromCenter >= degreeOuterRadius && radiusFromCenter < groupOuterRadius)
		mouseInGroupSector(mouseInDegreeSector(event, angle));

	return -1;
}

int GroupingCircle::mouseInGroupRingSector(const MouseEvent& event, float radiusFromCenter, int degreeIndex)
{
	if (radiusFromCenter >= degreeOuterRadius && radiusFromCenter < groupOuterRadius)
		mouseInGroupSector(degreeIndex);

	return -1;
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

int GroupingCircle::modulo(int num, int mod)
{
	return ((num % mod) + mod) % mod;
}
