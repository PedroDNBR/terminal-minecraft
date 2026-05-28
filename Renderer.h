#pragma once
#include <Windows.h>
#include <vector>

class Renderer
{
public:
	Renderer(int width, int height);
	bool hasWindowResized();
	void getWindowSize(int& width, int& height);
	void resizeWindow(int newWidth, int newHeight);
	void clear();
	void drawPixel(int x, int y, DWORD color);
	void present();

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

	float aspectRatio;
};

