#pragma once
namespace Platform
{
	void init();
	void shutdown();
	void getTerminalSize(int& width, int& height);
	void write(const char* data, size_t length);
}