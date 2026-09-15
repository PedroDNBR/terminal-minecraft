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
		width = 80; height = 25;
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

namespace
{
	constexpr int keyMap[] =
	{
		'W',
		'A', 
		'S', 
		'D',
		'P', 
		'B',
		'N',
		VK_ESCAPE,
		VK_RETURN,
		VK_SPACE,
		'Q',
		'I',
		'J', 
		'K', 
		'L'
	};
	static_assert(sizeof(keyMap) / sizeof(keyMap[0]) == static_cast<int>(Key::MAX),
		"Make sure the keyMap array has the same number of elements as the Key enum");
}

bool Platform::isKeyDown(Key key)
{
	const int vk = keyMap[static_cast<int>(key)];
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

#endif // _WIN32

