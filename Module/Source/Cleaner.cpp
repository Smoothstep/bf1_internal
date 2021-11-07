#include "stdafx.h"
#include "Cleaner.h"
#include "GameHooks.h"
#include "D3DHooks.h"
#include "FnValue.h"
#include <winnt.h>

#pragma comment(lib, "gdi32.lib")

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

auto Header = FnValue<const IMAGE_OPTIONAL_HEADER*>
{ 
    []()
    {
        return &reinterpret_cast<const IMAGE_NT_HEADERS*>(
            reinterpret_cast<uint8_t*>(&__ImageBase) + __ImageBase.e_lfanew)->OptionalHeader;
    } 
};

bool ScreenshotCleaner::NeedsFrame()
{
    if (LastTaken < LastRequested)
    {
        return true;
    }

    return false;
}

void ScreenshotCleaner::SetPolicy(EScreenshotPolicy NewPolicy)
{
    Policy = NewPolicy;
}

void ScreenshotCleaner::CaptureFrame()
{
    if (!BitBltArgs)
    {
        return;
    }

    if (std::apply(&BitBlt, *BitBltArgs))
    {
        LastTaken = Hrc::now();
        BitBltArgs = {};
    }
    else
    {
        Logger::Error() << "BitBlt failed: " << std::to_string(GetLastError()) << Logger::EndLine;
    }
}

BOOL ScreenshotCleaner::HkBitBlt(HDC Hdc, int X, int Y, int CX, int CY, HDC HdcSrc, int X1, int Y1, DWORD Rop)
{
    auto HkRef = HookRefSpec<D3D::GameInterface&>::Current();

    auto& [Gi] = HkRef.Tuple();
    auto& Cleaner = Gi.GetCleaner();

    const auto ReturnAddr = *reinterpret_cast<const size_t*>(_AddressOfReturnAddress());

    if (ReturnAddr > Header->ImageBase &&
        ReturnAddr < Header->ImageBase + Header->SizeOfImage &&
        Rop != ~0U)
    {
        return (*HkRef->GetOriginal<decltype(&BitBlt)>())(Hdc, X, Y, CX, CY, HdcSrc, X1, Y1, Rop != -1 ? Rop : SRCCOPY);
    }

    Logger::Info() << "BitBlt called" << Logger::EndLine;

    switch (Cleaner.Policy)
    {
        case EScreenshotPolicy::PolicyClean:
        {
            while (Cleaner.BitBltArgs)
            {
                std::this_thread::yield();
            }

            Cleaner.BitBltArgs = std::make_optional<FunctionArgs<decltype(&BitBlt)>>(std::make_tuple(Hdc, X, Y, CX, CY, HdcSrc, X1, Y1, Rop != -1 ? Rop : SRCCOPY));
            
            for (Cleaner.LastRequested = Hrc::now(); Cleaner.LastRequested > Cleaner.LastTaken;)
            {
                std::this_thread::yield();
            }

            return TRUE;
        }

        case EScreenshotPolicy::PolicyBlacken:
        {
            const RECT Rect{ X, Y, X + CX, Y + CY };
            HBRUSH Brush = CreateSolidBrush(0);
            FillRect(Hdc, &Rect, Brush);

            return TRUE;
        }

        case EScreenshotPolicy::PolicyIgnore:
        {
            return TRUE;
        }

        case EScreenshotPolicy::DoNothing:
        {
            break;
        }
    }

    return (*HkRef->GetOriginal<decltype(&BitBlt)>())(Hdc, X, Y, CX, CY, HdcSrc, X1, Y1, Rop);
}
