#include "Renderer.h"

Renderer::Renderer(int width, int height)
{
	realWidth = width;
	realHeight = height / 2;
	logicalWidth = width;
	logicalHeight = height;

	aspectRatio = (float)logicalWidth / logicalHeight * 0.55f;

	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	rect = {0,0, static_cast<SHORT>(width - 1), static_cast<SHORT>(height - 1)};

	colorBuffer.resize(width * height);
	depthBuffer.resize(width * height, 0);
}

void Renderer::drawPixel(int x, int y, DWORD color)
{
	if (x < 0 || x >= logicalWidth || y < 0 || y >= logicalHeight)
		return;

	colorBuffer[y * logicalWidth + x] = color;
}

void Renderer::drawPixelDepth(int x, int y, DWORD color, float inverseZ)
{
	if (x < 0 || x >= logicalWidth || y < 0 || y >= logicalHeight)
		return;

	int index = y * logicalWidth + x;
	if (inverseZ <= depthBuffer[index])
		return;

	depthBuffer[index] = inverseZ;
	colorBuffer[index] = color;
}

void Renderer::getWindowSize(int& width, int& height)
{
	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
	GetConsoleScreenBufferInfo(handle, &consoleScreenBufferInfo);

	width = consoleScreenBufferInfo.srWindow.Right - consoleScreenBufferInfo.srWindow.Left + 1;
	height = consoleScreenBufferInfo.srWindow.Bottom - consoleScreenBufferInfo.srWindow.Top + 1;
}

void Renderer::resizeWindow(int newWidth, int newHeight)
{
	realWidth = newWidth;
	realHeight = newHeight;

	logicalWidth = newWidth;
	logicalHeight = newHeight * 2;

	aspectRatio = (float)logicalWidth / logicalHeight * 0.55f;

	screenBuffer.resize(realWidth * realHeight);
	rect.Right = static_cast<SHORT>(realWidth - 1);
	rect.Bottom = static_cast<SHORT>(realHeight - 1);

	colorBuffer.resize(logicalWidth * logicalHeight);
	depthBuffer.resize(logicalWidth * logicalHeight, 0);
}

void Renderer::clear()
{
	int logicalSize = logicalWidth * logicalHeight;
	std::fill(colorBuffer.begin(), colorBuffer.end(), 0);
	std::fill(depthBuffer.begin(), depthBuffer.end(), 0.0f);
}

bool Renderer::hasWindowResized()
{
	int newWidth, newHeight;
	getWindowSize(newWidth, newHeight);

	if (newWidth != realWidth || newHeight != realHeight)
	{
		resizeWindow(newWidth, newHeight);
		return true;
	}

	return false;
}

void Renderer::present()
{
	for (int x = 0; x < realWidth; x++)
	for (int y = 0; y < realHeight; y++)
	{
		int topIndex = (y * 2) * logicalWidth + x;
		int bottomIndex = (y * 2 + 1) * logicalWidth + x;

		WORD topColor = colorBuffer[topIndex];
		WORD bottomColor = colorBuffer[bottomIndex];

		CHAR_INFO& cell = screenBuffer[y * realWidth + x];
		cell.Char.UnicodeChar = L'\u2584';
		cell.Attributes = (topColor << 4) | bottomColor;
	}

	WriteConsoleOutput(
		handle, 
		screenBuffer.data(), 
		{ static_cast<SHORT>(realWidth), static_cast<SHORT>(realHeight) },
		{0, 0},
		&rect
	);
}