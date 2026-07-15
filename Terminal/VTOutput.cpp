#include "VTOutput.h"
#include "../Platform/Platform.h"

void VTOutput::present(const std::vector<uint8_t>& colorBuffer, const std::vector<RawTextPrint>& textToPrint, int realWidth, int realHeight, int logicalWidth, int logicalHeight)
{
	outputFrame.clear();
	outputFrame += "\x1b[H";

	uint8_t lastTopColorIndex = 255;
	uint8_t lastBottomColorIndex = 255;

	for (int y = 0; y < realHeight; y++)
	for (int x = 0; x < realWidth; x++)
	{
		int topColor = (y * 2) * logicalWidth + x;
		int bottomColor = (y * 2 + 1) * logicalWidth + x;

		uint8_t topColorIndex = colorBuffer[topColor];
		uint8_t bottomColorIndex = colorBuffer[bottomColor];

		if (topColorIndex != lastTopColorIndex)
		{
			appendBackground(outputFrame, PALETTE[topColorIndex]);
			lastTopColorIndex = topColorIndex;
		}

		if (bottomColorIndex != lastBottomColorIndex)
		{
			appendForeground(outputFrame, PALETTE[bottomColorIndex]);
			lastBottomColorIndex = bottomColorIndex;
		}

		outputFrame += "\xE2\x96\x84";
	}

	for (const RawTextPrint& textPrint : textToPrint)
	{
		outputFrame += "\x1b[";
		outputFrame += std::to_string(textPrint.y + 1);
		outputFrame += ";";
		outputFrame += std::to_string(textPrint.x + 1);
		outputFrame += "H";

		appendForeground(outputFrame, PALETTE[textPrint.color]);
		appendBackground(outputFrame, PALETTE[BLACK]);

		outputFrame += textPrint.text;
	}
	outputFrame += "\x1b[0m";

	Platform::write(outputFrame.c_str(), outputFrame.size());
}

void VTOutput::appendByte(std::string& out, uint8_t byte)
{
	if (byte >= 100)
	{
		out += char('0' + byte / 100);
	}
	if (byte >= 10)
	{
		out += char('0' + byte / 10 % 10);
	}
	out += char('0' + byte % 10);
}

void VTOutput::appendForeground(std::string& out, uint32_t rgbColor)
{
	out += "\x1b[38;2;";
	appendByte(out, (rgbColor >> 16) & 0xFF);
	out += ";";
	appendByte(out, (rgbColor >> 8) & 0xFF);
	out += ";";
	appendByte(out, rgbColor & 0xFF);
	out += "m";
}

void VTOutput::appendBackground(std::string& out, uint32_t rgbColor)
{
	out += "\x1b[48;2;";
	appendByte(out, (rgbColor >> 16) & 0xFF);
	out += ";";
	appendByte(out, (rgbColor >> 8) & 0xFF);
	out += ";";
	appendByte(out, rgbColor & 0xFF);
	out += "m";
}
