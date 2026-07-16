#include <cmath>
#include "Renderer.h"
#include "../Core/Profiler.h"
#include "../Platform/Platform.h"

void Renderer::init()
{
	int width = 0;
	int height = 0;

	Platform::getTerminalSize(width, height);
	
	realWidth = width;
	realHeight = height;
	logicalWidth = width;
	logicalHeight = height * 2;

	aspectRatio = (float)logicalWidth / logicalHeight * 0.55f;

	resizeBuffers();
}

void Renderer::resizeBuffers()
{
	int logicalSize = logicalWidth * logicalHeight;
	colorBuffer.resize(logicalSize);
	depthBuffer.resize(logicalSize, 0);
}

void Renderer::drawPixel(int x, int y, uint8_t color)
{
	if (x < 0 || x >= logicalWidth || y < 0 || y >= logicalHeight)
		return;

	colorBuffer[y * logicalWidth + x] = color;
}

void Renderer::drawPixelDepth(int x, int y, float inverseZ, uint8_t color)
{
	if (x < 0 || x >= logicalWidth || y < 0 || y >= logicalHeight)
		return;

	int index = y * logicalWidth + x;
	if (inverseZ <= depthBuffer[index])
		return;

	depthBuffer[index] = inverseZ;
	colorBuffer[index] = color;
}

void Renderer::drawPolygonWireframe(Vertex* verts, int count, uint8_t color)
{
	for (int i = 0; i < count; i++)
	{
		int j = (i + 1) % count;

		float x0 = verts[i].viewPosition.x, y0 = verts[i].viewPosition.y, z0 = verts[i].inverseZ;
		float x1 = verts[j].viewPosition.x, y1 = verts[j].viewPosition.y, z1 = verts[j].inverseZ;

		float dx = x1 - x0;
		float dy = y1 - y0;
		float steps = (fabsf(dx) > fabsf(dy)) ? fabsf(dx) : fabsf(dy);
		if (steps < 1.0f) steps = 1.0f;

		float stepX = dx / steps;
		float stepY = dy / steps;
		float stepZ = (z1 - z0) / steps;

		float x = x0, y = y0, z = z0;
		int n = (int)steps;
		for (int s = 0; s <= n; s++)
		{
			int px = (int)(x + 0.5f);
			int py = (int)(y + 0.5f);
			if (px >= 0 && px < logicalWidth && py >= 0 && py < logicalHeight)
				drawPixelDepth(px, py, z, color);
			x += stepX;
			y += stepY;
			z += stepZ;
		}
	}
}

void Renderer::drawQuadWireframe(Vertex v0, Vertex v1, Vertex v2, Vertex v3, uint8_t color)
{
	Vertex verts[4] = { v0, v1, v2, v3 };
	drawPolygonWireframe(verts, 4, color);
}

void Renderer::drawFilledQuad(Vertex v0, Vertex v1, Vertex v2, Vertex v3, uint8_t color)
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

		int startX = (int)ceilf(xLeft - 0.5f);
		int endX = (int)ceilf(xRight - 0.5f) - 1;

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

void Renderer::queueText(int x, int y, const std::string& text, uint8_t color)
{
	textToPrint.push_back({ x, y, text, color });
}

void Renderer::getWindowSize(int& width, int& height)
{
	Platform::getTerminalSize(width, height);
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

		resizeBuffers();
		return true;
	}

	return false;
}
