#pragma once

#include <optional>

#include "stdafx.h"
#include "FnTraits.h"
#include "ClassDecl.h"

class ScreenshotCleaner
{
public:
	ScreenshotCleaner() = default;
	
	bool NeedsFrame();
	void CaptureFrame();
	void SetPolicy(EScreenshotPolicy Policy);

	static BOOL HkBitBlt(HDC Hdc, int X, int Y, int CX, int CY, HDC HdcSrc, int X1, int Y1, DWORD Rop);

private:
	EScreenshotPolicy Policy;
	Hrc::time_point LastTaken;
	Hrc::time_point LastRequested;
	std::optional<FunctionArgs<decltype(&BitBlt)>> BitBltArgs;
};