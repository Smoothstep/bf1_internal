#pragma once

#include "ClientGame.h"
#include "Offsets.h"

class StaticCameraData
{
	uint8_t Unk0;
	uint8_t Unk1;
	uint8_t V;
	uint8_t Unk2;
	uint8_t Pad[0x24];
	float Unk3;
	float Unk4;
	float Unk5;
	float Unk6;
	float Unk7;
	float Unk8;
	float Unk9;
	float Unk10;
	float Unk11;
	float Unk12;
	float Unk13;
	float Unk14;
	uint8_t Pad1[0x10];
	void* HudData;
	void* Unk15;
	void* Hud[5];
	uint8_t Pad2[0x10];
	uint32_t Unk17;
	float Unk18;
	float Unk19;
	float Unk20;
	float Unk21;
	uint8_t Pad3[0x24];
	size_t Unk22;
	size_t Unk23;

	virtual void F0() = 0;

public:
	StaticCameraData(const StaticCameraData& From)
	{
		// Override vtable on purpose
		memcpy(this, std::addressof(From), sizeof(StaticCameraData));

		Unk0 = 0x0A;
		Unk2 = 0xB1;
		Unk17 = 1;
		Unk22 = 0x4DB6346DEB2FCDC9;
		Unk23 = 0x14A8765F33D9ADAA;
	}
};

constexpr size_t CSize = sizeof(StaticCameraData);

class CameraCallback
{

};

class Camera
{
private:
	uint8_t Pad[0x150];
	CameraCallback* Callback;

public:
	CameraCallback* GetCallback()
	{
		return Callback;
	}

	void SetCallback(CameraCallback* Cb)
	{
		Callback = Cb;
	}
};

class StaticCamera : public Camera
{

};

class CameraHelper
{
public:
	static CameraHelper* Instance()
	{
		return reinterpret_cast<CameraHelper*>(0x0000000143803558);
	}
};

class InternalCameraMgr
{
	struct CameraSource
	{
		struct Unk1 {
			uint8_t Pad[0x28];
			Camera* Cam;
		};
		struct Unk0 {
			uint8_t Pad[0x48];
			Unk1* Ptr;
		};

		uint8_t Pad[0x10];
		Unk0* Unk0;

		Camera* GetUnderlyingCamera()
		{
			return Unk0->Ptr->Cam;
		}
		void SetUnderlyingCamera(Camera* Cam)
		{
			Unk0->Ptr->Cam = Cam;
		}
	};

	class CurrentCameraInfo
	{
	private:
		uint8_t Pad0[0x1A8];
		CameraSource* Source;
		uint8_t Pad1[0x50];
		Camera* CamPtr;

		virtual void F0() = 0;

	public:
		Camera* GetGam()
		{
			return CamPtr;
		}

		void SetCam(Camera* Cam)
		{
			if (Source)
			{
				Source->SetUnderlyingCamera(Cam);
			}
			CamPtr = Cam;
		}
	};

	class CameraContainer
	{
	public:
		CurrentCameraInfo* CameraInfo;
	};

private:
	CameraContainer* Container;

	virtual void F0() = 0;

public:
	static CurrentCameraInfo* GetCurrentCameraInfo()
	{
		auto* Mgr = reinterpret_cast<InternalCameraMgr*>(0x0000000143803330);

		if (Mgr)
		{
			auto* Cnt = Mgr->Container;

			if (Cnt)
			{
				return Cnt->CameraInfo;
			}
		}
	}

	template<class TCamera, class TData>
	static TCamera* CreateCam(TData* Data)
	{
		return (reinterpret_cast<TCamera * (__thiscall*)(void*, void*, TData*, PhysicsManager*, size_t, size_t)>(CastPtr(EFunctionOffsets::Offset_CreateCamera)))(
			CastPtr(0x0000000143803558),
			CastPtr(0x0000000143753260),
			Data,
			GameLevel::GetCurrentLevel()->PhysicsManager,
			255,
			0);
	}
};