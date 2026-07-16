#pragma once
#include <Windows.h>
#include <vector>
#include "../Core/Vertex.h"
#include "Colors.h"
#include "RawTextPrint.h"

class Renderer
{
public:
	void init();
	void resizeBuffers();
	bool hasWindowResized();
	void getWindowSize(int& width, int& height);
	void clear();
	void drawPixel(int x, int y, uint8_t color);
	void drawPixelDepth(int x, int y, float inverseZ, uint8_t color);
	void drawFilledQuad(Vertex v0, Vertex v1, Vertex v2, Vertex v3, uint8_t color);
	void drawQuadWireframe(Vertex v0, Vertex v1, Vertex v2, Vertex v3, uint8_t color);
	void drawPolygonWireframe(Vertex* verts, int count, uint8_t color);
	void queueText(int x, int y, const std::string& text, uint8_t color);

	uint8_t backgroundColor = Color::C_BLACK;

	float getAspectRatio()  const { return aspectRatio; }
	int getLogicalWidth()  const { return logicalWidth; }
	int getLogicalHeight() const { return logicalHeight; }
	int getRealWidth()     const { return realWidth; }
	int getRealHeight()    const { return realHeight; }

	const std::vector<uint8_t>& getColorBuffer() const { return colorBuffer; }
	const std::vector<RawTextPrint>& getTextToPrint() const { return textToPrint; }

	double packMs = 0;
	double writeMs = 0;

private:
	int logicalWidth;
	int logicalHeight;

	int realWidth;
	int realHeight;

	float aspectRatio;

	std::vector<uint8_t> colorBuffer;
	std::vector<float> depthBuffer;

	std::vector<RawTextPrint> textToPrint;
};

