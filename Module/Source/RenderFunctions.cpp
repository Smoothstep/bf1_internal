#include "stdafx.h"
#include "RenderFunctions.h"
#include "GameHooks.h"

size_t RenderFunctionDispatcher::DispatchAll(void* Context, RenderFunctionEntry* Entry, size_t NumFn, size_t Offset)
{
	size_t Result = 0;

	for (auto Current = Entry; NumFn > 0; Current = Current->Next(), NumFn -= Result)
	{
		Current->Execute(Offset, Context, Current);
		
		if (!(Result = Current->NextOffset))
		{
			break;
		}
	}

	return Result;
}
