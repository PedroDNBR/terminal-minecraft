
#include <windows.h>
#include <vector>
int main()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT rect = { 0, 0, 79, 49};
	SetConsoleWindowInfo(consoleHandle, TRUE, &rect);
	CONSOLE_SCREEN_BUFFER_INFOEX consoleScreenBufferInfoEx = { sizeof(consoleScreenBufferInfoEx) };
	GetConsoleScreenBufferInfoEx(consoleHandle, &consoleScreenBufferInfoEx);
	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo = { sizeof(consoleScreenBufferInfo) };
	GetConsoleScreenBufferInfo (consoleHandle, &consoleScreenBufferInfo);
	std::vector<CHAR_INFO> buffer(80 * 50);
	while (true)
	{
		for (auto& cell : buffer)
		{
			cell.Char.UnicodeChar = L'X';
			cell.Attributes = 14;
		}
		WriteConsoleOutput(consoleHandle, buffer.data(), { 80, 50 }, { 0, 0 }, &rect);
	}
}