#include "Renderer.h"

Renderer::Renderer(bool startFullscreen)
{
	handle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (startFullscreen)
	{
		ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
	}

	setupTerminalWindow();
}

void Renderer::setupTerminalWindow()
{
	int width = 0;
	int height = 0;

	getWindowSize(width, height);

	realWidth = width;
	realHeight = height;
	logicalWidth = width;
	logicalHeight = height * 2;

	aspectRatio = (float)logicalWidth / logicalHeight * 0.55f;

	rect = { 0,0, (SHORT)(realWidth - 1), (SHORT)(realHeight - 1) };

	resizeBuffers();
}

void Renderer::resizeBuffers()
{
	int logicalSize = logicalWidth * logicalHeight;
	colorBuffer.resize(logicalSize);
	depthBuffer.resize(logicalSize, 0);
	screenBuffer.resize(realWidth * realHeight);
}

void Renderer::drawPixel(int x, int y, WORD color)
{
	if (x < 0 || x >= logicalWidth || y < 0 || y >= logicalHeight)
		return;

	colorBuffer[y * logicalWidth + x] = color;
}

void Renderer::drawPixelDepth(int x, int y, float inverseZ, WORD color)
{
	if (x < 0 || x >= logicalWidth || y < 0 || y >= logicalHeight)
		return;

	int index = y * logicalWidth + x;
	if (inverseZ <= depthBuffer[index])
		return;

	depthBuffer[index] = inverseZ;
	colorBuffer[index] = color;
}

void Renderer::drawFilledQuad(Vertex v0, Vertex v1, Vertex v2, Vertex v3, WORD color)
{
	const int quadVertexCount = 4;
	float verticesX[quadVertexCount] = { v0.viewPosition.x, v1.viewPosition.x, v2.viewPosition.x, v3.viewPosition.x };
	float verticesY[quadVertexCount] = { v0.viewPosition.y, v1.viewPosition.y, v2.viewPosition.y, v3.viewPosition.y };
	float verticesZ[quadVertexCount] = { v0.inverseZ, v1.inverseZ, v2.inverseZ, v3.inverseZ };

	float minY = verticesY[0];
	float maxY = verticesY[0];

	for (int i = 1; i < quadVertexCount; i++)
	{
		if (verticesY[i] < minY) minY = verticesY[i];
		if (verticesY[i] > maxY) maxY = verticesY[i];
	}

	int yStart = (int)minY;
	int yEnd = (int)maxY;

	if (yStart < 0)
		yStart = 0;
	if (yEnd >= logicalHeight)
		yEnd = logicalHeight - 1;

	for (int y = yStart; y <= yEnd; y++)
	{
		float scanY = (float)y + .5f;

		float xLeft = 1e9f;
		float xRight = -1e9f;

		float inverseZLeft = 0.0f;
		float inverseZRight = 0.0f;

		for (int i = 0; i < quadVertexCount; i++)
		{
			int j = (i + 1) % quadVertexCount;
			float startLineX = verticesX[i];
			float startLineY = verticesY[i];
			float startLineZ = verticesZ[i];

			float endLineX = verticesX[j];
			float endLineY = verticesY[j];
			float endLineZ = verticesZ[j];

			if (
				(startLineY <= scanY && endLineY > scanY) ||
				(endLineY <= scanY && startLineY > scanY)
				)
			{
				float t = (scanY - startLineY) / (endLineY - startLineY);
				float interpolationX = startLineX + t * (endLineX - startLineX);
				float interpolationZ = startLineZ + t * (endLineZ - startLineZ);

				if (interpolationX < xLeft)
				{
					xLeft = interpolationX;
					inverseZLeft = interpolationZ;
				}
				if (interpolationX > xRight)
				{
					xRight = interpolationX;
					inverseZRight = interpolationZ;
				}
			}
		}
		if (xLeft > xRight) continue;

		int startX = (int)xLeft;
		int endX = (int)xRight;

		if (startX < 0)
			startX = 0;
		if (endX >= logicalWidth)
			endX = logicalWidth - 1;

		float lineLenght = xRight - xLeft;

		for (int x = startX; x <= endX; x++)
		{
			float t = (lineLenght < 1e-6f) ? 0.0f : ((x + 0.5f) - xLeft) / lineLenght;
			float invZ = inverseZLeft + t * (inverseZRight - inverseZLeft);
			drawPixelDepth(x, y, invZ, color);
		}
	}
}

void Renderer::queueText(int x, int y, const std::wstring& text, WORD color)
{
	textToPrint.push_back({ x, y, text, color });
}

void Renderer::drawText(int x, int y, const std::wstring& text, WORD color)
{
	if(y < 0 || y >= logicalHeight)
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

void Renderer::getWindowSize(int& width, int& height)
{
	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
	GetConsoleScreenBufferInfo(handle, &consoleScreenBufferInfo);

	width = consoleScreenBufferInfo.srWindow.Right - consoleScreenBufferInfo.srWindow.Left + 1;
	height = consoleScreenBufferInfo.srWindow.Bottom - consoleScreenBufferInfo.srWindow.Top + 1;
}

void Renderer::clear()
{
	int logicalSize = logicalWidth * logicalHeight;
	std::fill(colorBuffer.begin(), colorBuffer.end(), backgroundColor);
	std::fill(depthBuffer.begin(), depthBuffer.end(), 0.0f);
	textToPrint.clear();
}

bool Renderer::hasWindowResized()
{
	int newWidth, newHeight;
	getWindowSize(newWidth, newHeight);

	if (newWidth != realWidth || newHeight != realHeight)
	{
		realWidth = newWidth;
		realHeight = newHeight;

		logicalWidth = realWidth;
		logicalHeight = realHeight * 2;

		rect = { 0,0,
				 (SHORT)(realWidth - 1),
				 (SHORT)(realHeight - 1) };

		resizeBuffers();
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

		CHAR_INFO& cell = screenBuffer[y * realWidth + x];
		cell.Char.UnicodeChar = L'\u2584';
		cell.Attributes = (colorBuffer[topIndex] << 4) | colorBuffer[bottomIndex];
	}

	for (const auto& cmd : textToPrint)
	{
		drawText(cmd.x, cmd.y, cmd.text, cmd.color);
	}

	WriteConsoleOutput(
		handle,
		screenBuffer.data(),
		{ (SHORT)realWidth, (SHORT)realHeight },
		{ 0, 0 },
		&rect
	);
}