/*
  ==============================================================================

    ComboBoxLookAndFeel.h
    Created: 13 May 2020 9:13:28pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class ComboBoxLookAndFeel : public LookAndFeel_V4
{

public:

	~ComboBoxLookAndFeel() override {};

	void drawComboBox(
		Graphics& g, int width, int height, bool isButtonDown, 
		int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& cb)
	{
		Font font = getComboBoxFont(cb);
		String text = cb.getText();
		int textWidth = font.getStringWidth(text);
		int textHeight = font.getHeight();
		int marginF = 128;
		float margin = (marginF + 1) / marginF;

		if (cb.isMouseOver())
		{
			g.setColour(Colours::black.withAlpha(0.125f));
			g.fillRoundedRectangle((width / 2 - textWidth / 2) * margin, 0, textWidth * margin, textHeight, sqrt(width * height) / 8.0f);
		}
	}

	Font getComboBoxFont(ComboBox& cb)
	{
		return Font().withHeight(cb.getHeight());
	}

	Label* createComboBoxTextBox(ComboBox& cb)
	{
		Label* l = new Label(cb.getName() + "_label", cb.getText());
		l->setInterceptsMouseClicks(false, false);
		return l;
	}

	void positionComboBoxText(ComboBox& cb, Label& labelToPosition)
	{
		Font font = getComboBoxFont(cb);
		labelToPosition.setBounds(0, 0, cb.getWidth(), cb.getHeight());
		labelToPosition.setFont(font);
		labelToPosition.setJustificationType(Justification::centred);
	}

	PopupMenu::Options getOptionsForComboBoxPopupMenu(ComboBox& cb, Label& lbl)
	{
		return PopupMenu::Options()
			.withMinimumWidth(100)
			.withMaximumNumColumns(1)
			.withStandardItemHeight(cb.getHeight() * 2.0f / 3.0f)
			.withTargetComponent(cb);
	}
};