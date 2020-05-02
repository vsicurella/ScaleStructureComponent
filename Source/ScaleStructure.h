/*
  ==============================================================================

	ScaleStructure.h
	Created: 25 Jan 2020 5:12:32pm
	Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ScaleMath.h"

class ScaleStructure
{
	int period;
	int generatorIndex = -1;
	int periodFactorIndexSelected = 0;
	int sizeIndexSelected = -1;
	int generatorOffset = 0;

	int periodFactorSelected;
	int fPeriod; // The fractional period, if a period factor greater than one is chosen

	Array<int> periodFactors; // Used for fractional period scales
	Array<int> validGenerators; // Generators coprime with chosen period
	Array<int> scaleSizes; // Sizes supported by Period & Generator combo

	Array<Point<int>> keyboardTypes; // Erv Wilson's notation
	Array<PointPair<int>> pgCoords; // Hex Coordinates of period and generator
	Array<Point<int>> stepSizes; // Hex step sizes

	Array<int> generatorChain;
	Array<int> degreeGroupSizes;
	Array<Array<int>> degreeGroupings;

	Array<Point<int>> modmosProperties;

	// Calculates the properties related to the Period & Generator combo
	void calculateProperties();

	// Uses scale properties to determine hex step sizes
	void calculateStepSizes();

	// Enumerates the scale as a chain of generators
	void calculateGeneratorChain();

	// Fills arrays of degrees based on degreeGroupSizes. Use this if group sizes haven't been arranged symmetrically
	// Starts at beginning of generatorChain and puts the succeeding degrees in the second group, but wraps to degrees
	// preceding the generatorChain beginning into the third group, and alternates to fill all degrees
	void fillGroupingSymmetrically();

	// Takes an already 'symmetricized' group size array and fills groups of degrees based on this.
	void fillSymmetricGrouping();

	// Swaps degrees in generator chain based off of modifications
	void applyMODMOSProperties();

public:

	ScaleStructure(int periodIn);
	ScaleStructure(int periodIn, int genIndexIn, int sizeIndexIn, Array<int> degreeGroupsIn = Array<int>());
	ScaleStructure(const ScaleStructure& scaleToCopy);
	ScaleStructure(ValueTree scaleStructureProperties);

	/*
		Returns the chosen period. If 'true' is passed in, the period is divided by
		the factor, if a fractional period is selected.
	*/
	int getPeriod(bool ofSelectedFactor = false) const;
	void resetToPeriod(int periodIn);
	
	Array<int> getPeriodFactors() const;
	int getPeriodFactorIndex() const;
	int getPeriodFactor() const;

	Array<int> getValidGenerators() const;
	const Array<int>& getValidGeneratorsReference();
	int getValidGenerator(int genIndex) const;
	int getGenerator() const;
	int getGeneratorIndex() const;

	Array<int> getScaleSizes() const;
	int getScaleSize(int ind) const;

	/*
		Returns the currently selected scale size
	*/
	int getScaleSize() const;
	int getScaleSizeIndex() const;

	Array<Point<int>> getKeyboardTypes() const;
	Point<int> getKeyboardType(int ind) const;

	Array<PointPair<int>> getPGCoords() const;
	PointPair<int> getPGCoord(int ind) const;

	Point<int> getStepSizes(int kbdTypeIn) const;
	Point<int> getStepSize() const;

	Array<int> getGeneratorChain() const;
	const Array<int>& getGeneratorChainReference();

	Array<int> getGroupingSizes() const;
	const Array<int>& getGroupingSizesReference();

	Array<Array<int>> getDegreeGroupings() const;
	const Array<Array<int>>& getDegreeGroupingsReference() const;
	int getGroupOfDegree(int scaleDegreeIn) const;

	Array<Point<int>> getMODMOSProperties() const;
	int modeDegreeToScaleDegree(int modeDegree) const;
	int getAlterationOfDegree(int modeDegree) const;
	void setAlterationofDegree(int modeDegree, int alteration);

	bool isValid() const;

	void setPeriodFactorIndex(int index);
	void setGeneratorIndex(int index);
	void setSizeIndex(int index);
	void setGeneratorOffset(int offsetIn);

	/*
		Input a mapping of scale degrees and chroma alteration values.
		x of Point is a scale degree between 0 and scaleSize, in numerical order (not chain of generators)
		y of Point is the amount of chroma alterations, usually +/- 1
	*/
	void setMODMOSProperties(Array<Point<int>> modmosPropertiesIn);

	// Returns the index whose generator is closest to a perfect fifth
	int getSuggestedGeneratorIndex();

	// Returns the index whose size is closest to 7
	int getSuggestedSizeIndex();
	int getPrefferedSizeIndex(int prefferedSize, bool preferLarger = true);

	Array<int> getNestedSizeGrouping();
	Array<int> getComplimentarySizeGrouping();
	void useSuggestedSizeGrouping();
};