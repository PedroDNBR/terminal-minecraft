#pragma once
#include <Windows.h>
#include <vector>
#include "Vertex.h"
#include "RawTextPrint.h"

class Renderer
{
public:
	Renderer(bool startFullscreen = false);
	void setupTerminalWindow();
	void resizeBuffers();
	bool hasWindowResized();
	void getWindowSize(int& width, int& height);
	void clear();
	void drawPixel(int x, int y, WORD color);
	void drawPixelDepth(int x, int y, float inverseZ, WORD color);
	void drawFilledQuad(Vertex v0, Vertex v1, Vertex v2, Vertex v3, WORD color);
	void queueText(int x, int y, const std::wstring& text, WORD color);
	void present();

	WORD backgroundColor = 0;

	int getAspectRatio()  const { return aspectRatio; }
	int getLogicalWidth()  const { return logicalWidth; }
	int getLogicalHeight() const { return logicalHeight; }
	int getRealWidth()     const { return realWidth; }
	int getRealHeight()    const { return realHeight; }

private:
	HANDLE handle;
	SMALL_RECT rect;

	int logicalWidth;
	int logicalHeight;

	int realWidth;
	int realHeight;

	std::vector<CHAR_INFO> screenBuffer;
	std::vector<WORD> colorBuffer;
	std::vector<float> depthBuffer;

	std::vector<RawTextPrint> textToPrint;

	float aspectRatio;

	void drawText(int x, int y, const std::wstring& text, WORD color);
};

