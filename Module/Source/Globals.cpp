#include "stdafx.h"
#include "Globals.h"
#include "Offsets.h"

namespace Globals
{
	std::filesystem::path BaseDirectory = []()
	{
		char Dir[1024];
		GetCurrentDirectoryA(sizeof(Dir), Dir);
		return std::filesystem::path(Dir);
	}();

	HWND WindowHandle = []()
	{
		return *reinterpret_cast<HWND*>(EVariableOffsets::Offset_HWND);
	}();
};