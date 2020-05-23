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
GroupingCircle::GroupingCircle(const ScaleStructure& structureIn, Array<Colour>& groupColoursIn)
	:	scaleStructure(structureIn),
		colourTable(groupColoursIn)
{

}

GroupingCircle::~GroupingCircle()
{
	listeners.clear();
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

		if (i == groupSectorMouseOver)
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

			// TODO: improve condition
			if (degreeChainIndexToMod > -1 && degreeModCandidates.contains(degIndex))
			{
				// Maybe outline instead of recolor
				degreeColour = groupColour.contrasting(Colours::mediumvioletred, 1.0f / 3);
			}

			if (degIndex == degreeSectorMouseOver)
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

	groupRingWidth = getWidth() * groupWidthRatio;
	degreeRingWidth = getWidth() * degreeWidthRatio;

	groupOuterRadius = getWidth() * borderRatio / 2.0f;
	degreeOuterRadius = groupOuterRadius - groupRingWidth;
	degreeInnerRadius = degreeOuterRadius - degreeRingWidth;

	groupMiddleRadius = (groupOuterRadius + degreeOuterRadius) / 2.0f;
	degreeMiddleRadius = (degreeOuterRadius + degreeInnerRadius) / 2.0f;

	groupOuterCircleBounds = getBounds().toFloat().reduced(proportionOfWidth(1 - borderRatio));
	groupInnerCircleBounds = groupOuterCircleBounds.reduced(groupRingWidth);
	degreeInnerCircleBounds = groupInnerCircleBounds.reduced(degreeRingWidth);
	
	angleIncrement = 2 * double_Pi / generatorChain.size();
	angleHalf = angleIncrement / 2.0f;

	// determine circle offset, based off of middle angle of first degree group
	circleOffset = groupSizes[0] * angleIncrement / 2.0f;

	int groupIndex = 0;
	int groupDegreesPassed = 0;
	
	float degreeLabelSize = jmin(degreeRingWidth, float_Tau * degreeMiddleRadius / degreeLabels.size()) * sectorLabelSizeRatio;
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
	mouseRadius = event.position.getDistanceFrom(center);
	bool dirty = false;

	// If mouse is not in rings, remove highlights
	if (mouseRadius < degreeInnerRadius || mouseRadius > groupOuterRadius)
	{
		lastDegreeSectorMouseIn = degreeSectorMouseOver;
		degreeSectorMouseOver = -1;

		lastGroupSectorMouseIn = groupSectorMouseOver;
		groupSectorMouseOver = -1;
		dirty = true;
	}

	else
	{
		float angle = getNormalizedMouseAngle(event);
		int degreeIndex = mouseInDegreeSector(event, angle);

		// Check Degree Sectors
		if (mouseRadius < degreeOuterRadius)
		{
			if (degreeSectorMouseOver != degreeIndex)
			{
				lastDegreeSectorMouseIn = degreeSectorMouseOver;
				degreeSectorMouseOver = degreeIndex;
				dirty = true;
			}

			if (groupSectorMouseOver > -1)
			{
				lastGroupSectorMouseIn = groupSectorMouseOver;
				groupSectorMouseOver = -1;
				dirty = true;
			}
		}

		// Check Group Sectors
		else if (mouseRadius < groupOuterRadius)
		{
			int groupIndex = mouseInGroupSector(degreeIndex);

			if (groupSectorMouseOver != groupIndex)
			{
				lastGroupSectorMouseIn = groupSectorMouseOver;
				groupSectorMouseOver = groupIndex;
				dirty = true;
			}

			if (degreeSectorMouseOver > -1)
			{
				lastDegreeSectorMouseIn = degreeSectorMouseOver;
				degreeSectorMouseOver = -1;
				dirty = true;
			}
		}
	}

	if (dirty)
		repaint();
}

void GroupingCircle::mouseDown(const MouseEvent& event)
{
	bool cancelDegreeMods = true;

	// If mouse is on a group or degree section
	if (mouseRadius > degreeInnerRadius&& mouseRadius < groupOuterRadius)
	{
		// If mouse on a degree
		if (mouseRadius < degreeOuterRadius)
		{
			// Show menu
			if (event.mods.isRightButtonDown())
			{
				PopupMenu::Options options = PopupMenu::Options().withMaximumNumColumns(1);

				int& modDegree = degreeChainIndexToMod;
				int degreeMouseOn = degreeSectorMouseOver;
				auto findMods = [this]() {findDegreeModCandidates(); };
				auto redraw = [this]() {repaint(); };

				degreeMenu.clear();
				degreeMenu.addItem("Show modifiable degrees", true, false, [&modDegree, degreeMouseOn, findMods, redraw](void) {
					modDegree = degreeMouseOn;
					findMods();
					redraw();
				});

				degreeMenu.showMenuAsync(options);
				cancelDegreeMods = false;
			}
			else if (degreeModCandidates.contains(degreeSectorMouseOver))
			{
				listeners.call(&Listener::degreesSwapped, degreeChainIndexToMod, 0);
				updateGenerator();
			}
		}
	}

	if (cancelDegreeMods)
	{
		degreeChainIndexToMod = -1;
		degreeModCandidates.clear();
		repaint();
	}
}

void GroupingCircle::mouseDrag(const MouseEvent& event)
{
	mouseDownRadius = event.mouseDownPosition.getDistanceFrom(center);
	mouseRadius = event.position.getDistanceFrom(center);
	bool dirty = false;

	if (mouseDownRadius >= degreeInnerRadius && mouseDownRadius < degreeOuterRadius)
	{
		float angle = getNormalizedMouseAngle(event);
		int degreeIndex = mouseInDegreeSector(event, angle);

		if (degreeSectorMouseOver != degreeIndex)
		{
			int offset = degreeIndex - degreeSectorMouseOver + scaleStructure.getGeneratorOffset();

			if (offset > -1 && offset < scaleStructure.getScaleSize())
			{
				//generatorOffset.setValue(offset);
				listeners.call(&Listener::offsetChanged, offset);

				DBG("Moved by " + String(degreeIndex - degreeSectorMouseOver) + "\tNew offset: " + String(offset));
				dirty = true;
			}

			lastDegreeSectorMouseIn = degreeSectorMouseOver;
			degreeSectorMouseOver = degreeIndex;
			
		}
	}

	if (dirty)
	{
		updateGenerator();
	}
}

void GroupingCircle::addListener(Listener* listenerToAdd)
{
	listeners.add(listenerToAdd);
}

void GroupingCircle::removeListener(Listener* listenerToRemove)
{
	listeners.remove(listenerToRemove);
}

void GroupingCircle::updatePeriod()
{
	DBG("Circle: Updating");

	removeAllChildren();

	degreeLabels.clear();

	for (int i = 0; i < scaleStructure.getPeriod(); i++)
	{
		Label* l = degreeLabels.add(new Label());
		l->setJustificationType(Justification::centred);
		l->setInterceptsMouseClicks(false, false);
		addAndMakeVisible(l);
	}

	degreeSectorMouseOver = -1;
}

void GroupingCircle::updateGenerator()
{
	degreeGroupings = scaleStructure.getDegreeGroupings();

	// build generatorChain based on degreeGroupings
	generatorChain.clear();
	
	groupSizes.clear();
	String dbgstr = "";
	for (int g = 0; g < degreeGroupings.size(); g++)
	{
		const Array<int>& group = degreeGroupings.getReference(g);
		groupSizes.add(group.size());

		for (auto degree : group)
		{
			generatorChain.add(degree);
			dbgstr += String(degree) + ", ";
		}
	}

	DBG("CIRCLE: Chain: " + dbgstr);
	
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


	groupSectorMouseOver = -1;

	resized();
	repaint();
}

// TODO: move this to Scale Structure?
void GroupingCircle::findDegreeModCandidates()
{
	// update chroma steps
	stepsToChromas.clear();
	for (int i = scaleStructure.getScaleSizeIndex(); i < scaleStructure.getScaleSizes().size() - 1; i++)
	{
		// TODO: improve getter
		stepsToChromas.add(scaleStructure.getScaleSizes()[i]);
	}

	degreeModCandidates.clear();
	//for (int i = 0; i < stepsToChromas.size(); i++)
	//{
		//int step = stepsToChromas[i];
		int step = scaleStructure.getScaleSize();
		int deg = modulo(degreeChainIndexToMod + step, scaleStructure.getPeriod());

		// forward
		// TODO: improve condition
		while (deg >= step)
		{
			// TODO: improve the "notAlreadyThere" check
			degreeModCandidates.addIfNotAlreadyThere(deg);
			deg = modulo(deg + step, scaleStructure.getPeriod());
		}

		deg = modulo(degreeChainIndexToMod - step, scaleStructure.getPeriod());
		// backward
		while (deg >= step)
		{
			degreeModCandidates.addIfNotAlreadyThere(deg);
			deg = modulo(deg - step, scaleStructure.getPeriod());
		}
	//}

	String dbgstr = "";
	for (auto degIndex : degreeModCandidates)
	{
		dbgstr += String(generatorChain[degIndex]) + ", ";
	}

	DBG("DEGREE MOD CANDIDATES: " + dbgstr);
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
