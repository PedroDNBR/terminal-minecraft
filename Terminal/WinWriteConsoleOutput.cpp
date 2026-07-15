#include "WinWriteConsoleOutput.h"

void WinWriteConsoleOutput::present(const std::vector<uint8_t>& colorBuffer, const std::vector<RawTextPrint>& textToPrint, int realWidth, int realHeight, int logicalWidth, int logicalHeight)
{
	if (handle == nullptr)
	{
		handle = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleMode(handle, &starterMode);

		SetConsoleMode(handle, starterMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		SetConsoleOutputCP(CP_UTF8);
	}
		
	if (screenBuffer.size() != realWidth * realHeight)
	{
		screenBuffer.resize(realWidth * realHeight);
		rect = { 0,0,
			 (SHORT)(realWidth - 1),
			 (SHORT)(realHeight - 1) 
		};
	}

	for (int y = 0; y < realHeight; y++)
	for (int x = 0; x < realWidth; x++)
	{
		int topIndex = (y * 2) * logicalWidth + x;
		int bottomIndex = (y * 2 + 1) * logicalWidth + x;

		CHAR_INFO& cell = screenBuffer[y * realWidth + x];
		cell.Char.UnicodeChar = L'\u2584';
		cell.Attributes = (PALETTE[colorBuffer[topIndex]] << 4) | PALETTE[colorBuffer[bottomIndex]];
	}

	for (const auto& cmd : textToPrint)
	{
		drawText(cmd.x, cmd.y, cmd.text, cmd.color, realWidth, realHeight, logicalWidth, logicalHeight);
	}

	WriteConsoleOutput(
		handle,
		screenBuffer.data(),
		{ (SHORT)realWidth, (SHORT)realHeight },
		{ 0, 0 },
		&rect
	);
}

void WinWriteConsoleOutput::drawText(int x, int y, const std::string& text, uint8_t color, int realWidth, int realHeight, int logicalWidth, int logicalHeight)
{
	if (y < 0 || y >= logicalHeight)
		return;

	for (int i = 0; i < text.size(); i++)
	{
		int positionX = x + i;

		if (positionX < 0 || positionX >= logicalWidth)
			return;

		CHAR_INFO& cell = screenBuffer[y * realWidth + positionX];

		cell.Char.UnicodeChar = text[i];
		cell.Attributes = color;
	}
}
