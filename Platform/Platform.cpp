#include "Platform.h"

#ifdef _WIN32

#include <Windows.h>

namespace {
	HANDLE handle;
	DWORD starterMode;
}

void Platform::init()
{
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(handle, &starterMode);

	SetConsoleMode(handle, starterMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	SetConsoleOutputCP(CP_UTF8);

	Platform::write("\x1b[?25l", 6);
}

void Platform::shutdown()
{
	Platform::write("\x1b[0m\x1b[?25h", 10);

	SetConsoleMode(handle, starterMode);
}

void Platform::getTerminalSize(int& width, int& height)
{
	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
	if (!GetConsoleScreenBufferInfo(handle, &consoleScreenBufferInfo)) {
		width = 80; height = 25;   // fallback + dá pra pôr breakpoint aqui
		return;
	}

	width = consoleScreenBufferInfo.srWindow.Right - consoleScreenBufferInfo.srWindow.Left + 1;
	height = consoleScreenBufferInfo.srWindow.Bottom - consoleScreenBufferInfo.srWindow.Top + 1;
}

void Platform::write(const char* data, size_t length)
{
	DWORD written;
	WriteFile(handle, data, (DWORD)length, &written, nullptr);
}

#endif // _WIN32

