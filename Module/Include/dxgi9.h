#pragma once

#include "InputController.h"
#include "D3DHooks.h"
#include "Hooks.h"
#include "ClassDecl.h"

class MainRoutine : InputSink
{
public:
	 MainRoutine(LPVOID pModule);
	~MainRoutine();

	DWORD MainThread();

private:
	HookDb Hooks;
	HMODULE ThisModule;
	D3D::GameInterface GameInterface;

	virtual bool Handle(LPMSG Msg) override;
};
