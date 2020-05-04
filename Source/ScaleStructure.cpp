
/*
  ==============================================================================

	ScaleStructure.cpp
	Created: 25 Jan 2020 5:12:32pm
	Author:  Vincenzo

  ==============================================================================
*/

#include "ScaleStructure.h"

ScaleStructure::ScaleStructure(int periodIn)
{
	resetToPeriod(periodIn);
}

ScaleStructure::ScaleStructure(int periodIn, int genIndexIn, int sizeIndexIn, Array<int> degreeGroupsIn)
{
	resetToPeriod(periodIn);
	setGeneratorIndex(genIndexIn);
	setSizeIndex(sizeIndexIn);
	
	if (degreeGroupsIn.size() > 0)
	{
		degreeGroupIndexedSizes = degreeGroupsIn;
		// TODO: check and fill
	}
	else
	{
		useSuggestedSizeGrouping();
	}
}

int ScaleStructure::getPeriod(bool ofFactorSelected) const
{
	if (ofFactorSelected && periodFactorSelected > 0)
		return fPeriod;

	return period;
}

void ScaleStructure::resetToPeriod(int periodIn)
{
	period = periodIn;
	generatorIndex = -1;
	sizeIndexSelected = -1;

	periodFactors = getFactors(period);
	setPeriodFactorIndex(0);
}

Array<int> ScaleStructure::getPeriodFactors() const
{
	return periodFactors;
}

int ScaleStructure::getPeriodFactorIndex() const
{
	return periodFactorIndexSelected;
}

int ScaleStructure::getPeriodFactor() const
{
	return periodFactorSelected;
}

Array<int> ScaleStructure::getValidGenerators() const
{
	return validGenerators;
}

const Array<int>& ScaleStructure::getValidGeneratorsReference()
{
	return validGenerators;
}

int ScaleStructure::getValidGenerator(int genIndex) const
{
	return validGenerators[genIndex];
}

int ScaleStructure::getGenerator() const
{
	return validGenerators[generatorIndex];
}

int ScaleStructure::getGeneratorIndex() const
{
	return generatorIndex;
}

Array<int> ScaleStructure::getScaleSizes() const
{
	return scaleSizes;
}

int ScaleStructure::getScaleSize(int ind) const
{
	return scaleSizes[ind];
}

int ScaleStructure::getScaleSize() const
{
	return scaleSizes[sizeIndexSelected];
}

int ScaleStructure::getScaleSizeIndex() const
{
	return sizeIndexSelected;
}

Array<Point<int>> ScaleStructure::getKeyboardTypes() const
{
	return keyboardTypes;
}

Point<int> ScaleStructure::getKeyboardType(int ind) const
{
	return keyboardTypes[ind];
}

Array<PointPair<int>> ScaleStructure::getPGCoords() const
{
	return pgCoords;
}

PointPair<int> ScaleStructure::getPGCoord(int ind) const
{
	return pgCoords[ind];
}

Point<int> ScaleStructure::getStepSizes(int kbdTypeIn) const
{
	return stepSizes[kbdTypeIn];
}

Point<int> ScaleStructure::getStepSize() const
{
	return stepSizes[sizeIndexSelected];
}

Array<int> ScaleStructure::getGeneratorChain() const
{
	return generatorChain;
}

const Array<int>& ScaleStructure::getGeneratorChainReference()
{
	return generatorChain;
}

Array<int> ScaleStructure::getGroupingIndexedSizes() const
{
	return degreeGroupIndexedSizes;
}

const Array<int>& ScaleStructure::getGroupingSizesReference()
{
	return degreeGroupScaleSizes;
}

Array<Array<int>> ScaleStructure::getDegreeGroupings() const
{
	return degreeGroupings;
}

const Array<Array<int>>& ScaleStructure::getDegreeGroupingsReference() const
{
	return degreeGroupings;
}

int ScaleStructure::getGroupOfDegree(int scaleDegreeIn) const
{
	for (int g = 0; g < degreeGroupIndexedSizes.size(); g++)
	{
		if (degreeGroupings.getReference(g).contains(scaleDegreeIn))
			return g;
	}

	return -1;
}

Array<Point<int>> ScaleStructure::getMODMOSProperties() const
{
	return modmosProperties;
}

int ScaleStructure::modeDegreeToScaleDegree(int modeDegree) const
{
	modeDegree = modulo(modeDegree, getScaleSize());
	Array<int> naturalScale = degreeGroupings[0];
	naturalScale.sort();

	return naturalScale[modeDegree];
}

int ScaleStructure::getAlterationOfDegree(int modeDegree) const
{
	for (auto alteration : modmosProperties)
	{
		if (alteration.x == modeDegree)
		{
			return alteration.y;
		}
	}

	return 0;
}

void ScaleStructure::setAlterationofDegree(int modeDegree, int alteration)
{
	int currentAlteration = getAlterationOfDegree(modeDegree);

	// Apply new MODMOS property
	if (currentAlteration != alteration)
	{
		int index = modmosProperties.indexOf(Point<int>(modeDegree, currentAlteration));

		if (alteration == 0 && index >= 0)
		{
			modmosProperties.removeRange(index, 1);
		}
		else
		{
			if (index < 0)
			{
				modmosProperties.add(Point<int>(modeDegree, alteration));
			}
			else
			{
				modmosProperties.set(index, Point<int>(modeDegree, alteration));
			}
		}

		String dbgstr = "";

		for (auto alteration : modmosProperties)
		{
			dbgstr += alteration.toString() + '\n';
		}
		DBG("MODMOS Properties:\n" + dbgstr);
	}

	fillSymmetricGrouping();
}

void ScaleStructure::setPeriodFactorIndex(int index)
{
	periodFactorIndexSelected = index;
	periodFactorSelected = periodFactors[periodFactorIndexSelected];
	fPeriod = period / periodFactorSelected;
	validGenerators = getCoprimes(fPeriod);

	if (generatorIndex > -1)
		calculateProperties();
}

void ScaleStructure::setGeneratorIndex(int index)
{
	generatorIndex = index;
	calculateProperties();
}

void ScaleStructure::setSizeIndex(int index)
{
	sizeIndexSelected = index;
	useSuggestedSizeGrouping();
}

void ScaleStructure::setGeneratorOffset(int offsetIn)
{
	generatorOffset = offsetIn;
	calculateGeneratorChain();
	useSuggestedSizeGrouping();
}

void ScaleStructure::setMODMOSProperties(Array<Point<int>> modmosPropertiesIn)
{
	modmosProperties = modmosPropertiesIn;
	fillSymmetricGrouping();
	// TODO: handle non-symmetric cases
}

void ScaleStructure::calculateProperties()
{
	// Clear all data dependent on Generator and Size choices
	scaleSizes.clear();
	keyboardTypes.clear();
	pgCoords.clear();
	modmosProperties.clear();

	// Calculate properties of scale
	Array<int> cf = getContinuedFraction((double)getGenerator() / fPeriod);

	// seed the sequence
	Point<int> parent1 = Point<int>(-1 + cf[0], 1);
	Point<int> parent2 = Point<int>(1, 0);
	Point<int> gp = Point<int>(cf[0], 1);

	Array<Point<int>> packet = { gp, parent2, gp + parent2 }; // makes for cleaner code
	pgCoords.add(PointPair<int>(packet[0], packet[1]));
	keyboardTypes.add(packet[2]);
	scaleSizes.add(packet[2].y);

	// find keyboard types, and their period/generator coordinates
	for (int i = 1; i < cf.size(); i++)
	{
		for (int d = 0; d < cf[i]; d++)
		{
			pgCoords.add(PointPair<int>(packet[0], packet[1]));
			keyboardTypes.add(packet[2]);
			scaleSizes.add(packet[2].y);

			parent1 = packet[0];
			parent2 = packet[1];
			gp = packet[2];

			// if previous continued fraction index is even,
			// set parent2 to previous result
			if (i % 2 == 0)
				parent1 = gp;
			else // if odd, set parent1 to previous result
				parent2 = gp;

			packet = { parent1, parent2, parent1 + parent2 };
		}
	}

	String dbgstr = "";
	for (auto s : keyboardTypes)
	{
		dbgstr += "(" + s.toString() + "), ";
	}

	DBG("Sizes available: " + dbgstr);

	calculateStepSizes();
	calculateGeneratorChain();
}

void ScaleStructure::calculateStepSizes()
{
	stepSizes.clear();

	Point<int> stepSizesOut;
	Point<int> periodCoordinate;
	Point<int> generatorCoordinate;

	int gen = getGenerator();

	for (int i = 0; i < scaleSizes.size(); i++)
	{
		generatorCoordinate = Point<int>(pgCoords[i].x.x, pgCoords[i].y.x);
		periodCoordinate = Point<int>(pgCoords[i].x.y, pgCoords[i].y.y);

		// find horiztonal step size (X)
		if (periodCoordinate.y == generatorCoordinate.y)
			stepSizesOut.setX(fPeriod - gen);
		else if (periodCoordinate.y == 0)
			stepSizesOut.setX(fPeriod);
		else if (generatorCoordinate.y == 0)
			stepSizesOut.setX(gen);
		else
			stepSizesOut.setX(fPeriod * generatorCoordinate.y - gen * periodCoordinate.y);

		// find upward right step size (Y)
		if (periodCoordinate.x == generatorCoordinate.x)
			stepSizesOut.setY(fPeriod - gen);
		else if (periodCoordinate.x == 0)
			stepSizesOut.setX(fPeriod);
		else if (generatorCoordinate.y == 0)
			stepSizesOut.setX(gen);
		else
			stepSizesOut.setY(gen * periodCoordinate.x - fPeriod * generatorCoordinate.x);

		stepSizes.add(stepSizesOut);
	}
}

void ScaleStructure::calculateGeneratorChain()
{
	generatorChain.clear();
	int gen = getGenerator();

	String dbgstr = "";

	for (int i = 0; i < fPeriod; i++)
	{
		generatorChain.add(modulo((i - generatorOffset) * gen, fPeriod));
		dbgstr += String(generatorChain[i]) + ", ";
	}

	// fill rest of degrees if fractional period is in place
	for (int i = 1; i < periodFactorSelected; i++)
	{
		for (int deg = 0; deg < fPeriod; deg++)
		{
			generatorChain.add(generatorChain[deg] + fPeriod * i);
			dbgstr += String(generatorChain[deg + fPeriod * i]) + ", ";
		}
	}

	DBG("Generator Chain: " + dbgstr);
}

void ScaleStructure::fillGroupingSymmetrically()
{
	degreeGroupings.clear();

	Array<int> grouping;

	for (int i = 0; i < periodFactorSelected; i++)
	{
		grouping.addArray(degreeGroupIndexedSizes);
	}

	degreeGroupings.resize(grouping.size());

	// Fill degree groups symmetrically

	int indexForward = 0;
	int indexBackwards = period - 1;
	int ind;
	for (int t = 0; t < grouping.size(); t++)
	{
		for (int n = 0; n < scaleSizes[grouping[t]]; n++)
		{
			if (t % 2 == 0)
			{
				ind = modulo(indexForward, period);
				indexForward++;
			}
			else
			{
				ind = modulo(indexBackwards, period);
				indexBackwards--;
			}

			degreeGroupings.getReference(t).add(generatorChain[ind]);
		}
	}

	String dbgstr = "";
	int size, sum = 0;
	for (int i = 0; i < grouping.size(); i++) {
		size = scaleSizes[grouping[i]];
		dbgstr += String(size) + ", ";
		sum += size;
	}
	dbgstr += " = " + String(sum);
	DBG("Using this size grouping: " + dbgstr);

	dbgstr = "";
	for (int group = 0; group < grouping.size(); group++)
	{
		Array<int> degreeGroup = degreeGroupings[group];
		dbgstr += "Tier " + String(group) + ": ";
		for (int deg = 0; deg < degreeGroup.size(); deg++)
		{
			dbgstr += String(degreeGroup[deg]) + ", ";
		}
		dbgstr += "\n";
	}

	DBG("Degree groupings: ");
	DBG(dbgstr);
}

void ScaleStructure::fillSymmetricGrouping()
{
	degreeGroupings.clear();
	degreeGroupings.resize(degreeGroupIndexedSizes.size());

	// Fill degree groups symmetrically

	int ind = 0;
	for (int t = 0; t < degreeGroupIndexedSizes.size(); t++)
	{
		for (int n = 0; n < scaleSizes[degreeGroupIndexedSizes[t]]; n++)
		{
			for (int f = 0; f < periodFactorSelected; f++)
			{
				degreeGroupings.getReference(t).add(generatorChain[ind + fPeriod * f]);
			}
			ind = modulo(ind + 1, fPeriod);
		}
	}

	// Rearrange degrees if MODMOS properties exist
	if (modmosProperties.size() > 0)
		applyMODMOSProperties();

	String dbgstr = "";
	int size, sum = 0;
	for (int i = 0; i < degreeGroupIndexedSizes.size(); i++) {
		size = scaleSizes[degreeGroupIndexedSizes[i]];
		dbgstr += String(size) + ", ";
		sum += size;
	}
	dbgstr += " = " + String(sum);
	DBG("Using this size grouping: " + dbgstr);

	dbgstr = "";
	for (int group = 0; group < degreeGroupIndexedSizes.size(); group++)
	{
		Array<int> degreeGroup = degreeGroupings[group];
		dbgstr += "Tier " + String(group) + ": ";
		for (int deg = 0; deg < degreeGroup.size(); deg++)
		{
			dbgstr += String(degreeGroup[deg]) + ", ";
		}
		dbgstr += "\n";
	}

	DBG("Degree groupings: ");
	DBG(dbgstr);
}

// TODO: Fix issues with generator offsets (modes).
//		- This algorithm finds and uses absolute degrees, which get affected by generator offset
//		- The proper solution would be to use relative degrees within the groups
//		- AKA swap values of degreeGrouping indicies, rather than values of generatorChain indicies
void ScaleStructure::applyMODMOSProperties()
{
	Array<int> naturalScale = degreeGroupings[0];
	naturalScale.sort();
	int fractionalPeriod = period / periodFactors[periodFactorIndexSelected];

	for (auto alteration : modmosProperties)
	{
		// Get current properties
		int naturalDegree = alteration.x;
		int scaleDegree = naturalScale[naturalDegree];
		int naturalGroupingIndex = degreeGroupings.getReference(0).indexOf(scaleDegree);

		int amount = alteration.y;
		int scaleSize = scaleSizes[sizeIndexSelected];

		// Find the altered scale degree
		int originalChainIndex = generatorChain.indexOf(scaleDegree);
		int shiftAmount = amount * scaleSize;
		int shiftedIndex = generatorChain.indexOf(scaleDegree) + amount * scaleSize;
		int newGeneratorIndex = modulo(generatorChain.indexOf(scaleDegree) + amount * scaleSize, fractionalPeriod);
		int alteredDegree = generatorChain[newGeneratorIndex];

		// Swap the scale degrees in degreeGroupings
		for (int i = 0; i < degreeGroupings.size(); i++)
		{
			Array<int>& group = degreeGroupings.getReference(i);

			if (group.contains(alteredDegree))
			{
				int indexToSwap = group.indexOf(alteredDegree);
				degreeGroupings.getReference(0).set(naturalGroupingIndex, alteredDegree);
				group.set(indexToSwap, scaleDegree);
				continue;
			}
		}
	}
}


int ScaleStructure::getSuggestedGeneratorIndex()
{
	int index = -1;
	float dif1, dif2 = 10e6;
	float interval = 1200.0f / fPeriod;
	int suggestedCents = 700;

	for (int i = 1; i < validGenerators.size(); i++)
	{
		dif1 = abs(suggestedCents - interval * validGenerators[i]);

		if (dif1 < dif2)
		{
			dif2 = dif1;
			index = i;
		}
	}

	return index;
}

int ScaleStructure::getSuggestedSizeIndex()
{
	int index = -1;
	int dif1, dif2 = INT_MAX;
	int scaleSize = 7 / periodFactorSelected;

	for (int i = 0; i < scaleSizes.size(); i++)
	{
		dif1 = abs(scaleSize - scaleSizes[i]);

		if (dif1 < dif2)
		{
			dif2 = dif1;
			index = i;
		}
	}

	return index;
}

int ScaleStructure::getPrefferedSizeIndex(int prefferedSize, bool preferLarger)
{
	int index = -1;
	int dif1, dif2 = INT_MAX;
	int scaleSize = prefferedSize / periodFactorSelected;

	for (int i = 0; i < scaleSizes.size(); i++)
	{
		dif1 = abs(scaleSize - scaleSizes[i]);

		if (dif1 < dif2 || (preferLarger && dif1 == dif2))
		{
			dif2 = dif1;
			index = i;
		}
	}

	return index;
}

Array<int> ScaleStructure::getNestedSizeGrouping()
{
	int scaleSize = getScaleSize();
	Array<int> grouping = { sizeIndexSelected };

	int notesLeft = fPeriod - scaleSize;
	int subSizeInd = sizeIndexSelected;
	int subSize = scaleSize;

	while (notesLeft > 0)
	{
		int q = notesLeft / subSize;

		if (notesLeft <= subSize && scaleSizes.contains(notesLeft))
		{
			grouping.add(scaleSizes.indexOf(notesLeft));
			notesLeft = 0;
		}
		else if (q >= 2)
		{
			int numToAdd = notesLeft % subSize == 0 ? q : q - (q % 2);
			for (int n = 0; n < numToAdd; n++)
			{
				grouping.add(subSizeInd);
				notesLeft -= subSize;

				// check if notesLeft can be divided equally by next size
				if (subSizeInd > 0)
				{
					int stagedSubInd = subSizeInd - 1;
					int stagedSubSize = scaleSizes[stagedSubInd];
					int q2 = notesLeft / stagedSubSize;

					if (notesLeft % stagedSubSize == 0)
					{
						// extra check for symmetry
						if (q2 % 2 == 0)
						{
							for (int qn = 0; qn < q2; qn++)
							{
								grouping.add(stagedSubInd);
								notesLeft -= stagedSubSize;
							}

							break;
						}
					}
				}
			}
		}

		subSize = scaleSizes[--subSizeInd];
	}

	DBG("Nested group:");
	String dbgstr = "";
	for (int i = 0; i < grouping.size(); i++)
		dbgstr += String(scaleSizes[grouping[i]]) + ", ";
	DBG(dbgstr);

	return grouping;
}

// TODO: possibility for improving large scales groupings.
// When checking the "staged" sizes, there could be an additional check to see if
// a different configuration of those two sizes produce a smaller grouping
// ex. with this algorithm, "Schismatic" 69/118 scale gives (7 * 4) + (5 * 18) or 22 groups
// but this could be reduced to (7 * 14) + (5 * 4) for 18 groups
Array<int> ScaleStructure::getComplimentarySizeGrouping()
{
	int scaleSize = getScaleSize();
	Array<int> grouping = { sizeIndexSelected };

	int notesLeft = fPeriod - scaleSize;
	int subSizeInd = sizeIndexSelected;
	int subSize = scaleSize;

	int q = notesLeft / subSize;
	int numToAdd = notesLeft % subSize == 0 ? q : q - (q % 2);
	int num = 0;

	while (notesLeft > 0)
	{
		// check if notesLeft can be divided equally by next size
		if (subSizeInd > 0)
		{
			int stagedSubInd = subSizeInd - 1;
			int stagedSubSize = scaleSizes[stagedSubInd];
			int q2 = notesLeft / stagedSubSize;

			if (notesLeft % stagedSubSize == 0)
			{
				// extra check for symmetry
				int groupSize = grouping.size() - 1;
				if (groupSize == 0 || q2 % 2 == 0)
				{
					for (int qn = 0; qn < q2; qn++)
					{
						grouping.add(stagedSubInd);
						notesLeft -= stagedSubSize;
					}

					break;
				}
			}
		}

		if (notesLeft <= subSize && scaleSizes.contains(notesLeft))
		{
			grouping.add(scaleSizes.indexOf(notesLeft));
			notesLeft = 0;
		}
		else if (q >= 0 && num < numToAdd)
		{
			grouping.add(subSizeInd);
			notesLeft -= subSize;
			num++;
		}
		else if (notesLeft > 0)
		{
			subSize = scaleSizes[--subSizeInd];
			q = notesLeft / subSize;
			numToAdd = notesLeft % subSize == 0 ? q : q - (q % 2);
			num = 0;
		}
	}

	DBG("Complimentary group:");
	String dbgstr = "";
	for (int i = 0; i < grouping.size(); i++)
		dbgstr += String(scaleSizes[grouping[i]]) + ", ";
	DBG(dbgstr);

	return grouping;
}

void ScaleStructure::useSuggestedSizeGrouping()
{
	Array<Array<int>> groupings;
	groupings.add(getNestedSizeGrouping());
	groupings.add(getComplimentarySizeGrouping());

	Array<float> groupingScores;

	// create score by averaging range and length of group
	for (auto g : groupings)
	{
		int range = g[0] - g[g.size() - 1];
		groupingScores.add((range + g.size()) / (float)groupings.size());
	}

	// find lowest score and return respective group
	int index = 0;
	for (int s = 0; s < groupings.size(); s++)
	{
		if (groupingScores[s] < groupingScores[index])
			index = s;
	}

	// make symmetric and fill degree groupings
	degreeGroupIndexedSizes = arrangeSymmetrically(groupings[index]);

	// fill scale size group
	degreeGroupScaleSizes.clear();
	for (auto i : degreeGroupIndexedSizes)
	{
		degreeGroupScaleSizes.add(scaleSizes[i]);
	}

	DBG("Symmetric group:");
	String dbgstr = "";
	for (int i = 0; i < degreeGroupIndexedSizes.size(); i++)
		dbgstr += String(scaleSizes[degreeGroupIndexedSizes[i]]) + ", ";
	DBG(dbgstr);

	fillSymmetricGrouping();
}

bool ScaleStructure::isValid() const
{
	if (generatorIndex < 0 || generatorIndex > validGenerators.size())
	{
		DBG("Invalid generator");
		return false;
	}

	if (sizeIndexSelected < 0 || sizeIndexSelected >= scaleSizes.size())
	{
		DBG("Invalid scale size");
		return false;
	}

	int sum = 0;
	for (int s = 0; s < degreeGroupIndexedSizes.size(); s++)
	{
		sum += scaleSizes[degreeGroupIndexedSizes[s]] * periodFactorSelected;
	}

	if (sum != period)
	{
		DBG("Invalid scale groupings");
		return false;
	}

	return true;
}

String ScaleStructure::getIntervalSteps(Point<int>& stepSizesOut)
{
	Array<int> sizes;
	for (int i = 0; i < getScaleSize(); i++)
	{
		for (int p = 0; p < periodFactorSelected; p++)
			sizes.add(generatorChain[i + fPeriod * p]);
	}

	sizes.add(period);
	sizes.sort();

	String steps = "";
	for (int i = 1; i <= getScaleSize(); i++)
	{
		sizes.set(i - 1, sizes[i] - sizes[i - 1]);
		steps += String(sizes[i - 1]) + " ";
	}

	// Extract step sizes
	stepSizesOut.x = sizes[0];
	for (auto step : sizes)
	{
		if (step != stepSizesOut.x)
		{
			stepSizesOut.y = step;
			break;
		}
	}

	DBG("ScaleStructure: MOS scale step sizes are: " + stepSizesOut.toString());

	return steps.substring(0, steps.length() - 1);
}

String ScaleStructure::getIntervalSteps()
{
	Point<int> dummySteps;
	return getIntervalSteps(dummySteps);
}

String ScaleStructure::getLsSteps()
{
	Point<int> sizes;
	String steps = getIntervalSteps(sizes);
	
	String L = String(jmax(sizes.x, sizes.y));
	String s = String(jmin(sizes.x, sizes.y));

	// TODO: make more efficent
	while (steps.containsWholeWord(L))
		steps = steps.replaceFirstOccurrenceOf(L, "L");

	while (steps.containsWholeWord(s))
		steps = steps.replaceFirstOccurrenceOf(s, "s");

	return steps;
}