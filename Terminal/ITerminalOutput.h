#include <vector>
#include "RawTextPrint.h"

#pragma once
struct ITerminalOutput
{
	virtual void present(
		const std::vector<uint8_t>& colorBuffer, 
		const std::vector<RawTextPrint>& textToPrint,
		int logicalWidth, int logicalHeight, 
		int realWidth, int realHeight) = 0;
	~ITerminalOutput() = default;
};