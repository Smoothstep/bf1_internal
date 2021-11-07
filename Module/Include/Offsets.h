#pragma once

#include <stdint.h>

enum class EInstanceOffsets : size_t
{
	Offset_CryptoMgr = 0x14351D058, // 48 39 EB 74 7B 0F 1F 84 00 00 00 00 00 48 8D 4B 20 48 8B 07 48 89 4C 24 28 48 89 44 24 30 F0 FF 41 10 F7 41 0C 00 00 00 80 75 3C 48 8D 44 24 28 48 89 07 4C 8B 01 4D 85 C0,
	Offset_GameRenderer = 0x1439e6d08, // 48 8B 80 60 09 00 00 49 89 06 48 8B 00 49 89 5E 18 49 89 7E 20 49 89 46 10 8B 86 90 32 02 00 41 89 46 3C 8B 86 94 32 02 00 41 89 46 38 48 8B 0D 50 09 0B FC 48 8B 01 FF 90 A0 00 00 00 31 D2 0F 10 80 60 04 00 00 41 0F 29 46 40 0F 10 88 70 04 00 00 41 0F 29 4E 50 0F 10 80 80 04 00 00 41 0F 29 46 60 0F 10 88 90 04 00 00 41 0F 29 4E 70
	Offset_GameCContext = 0x1437f7758, // 40 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 48 48 8B 05 1D B4 03 02 48 85 C0 0F 84 EB 00 00 00 48 8B 78 68 48 85 FF 0F 84 DE 00 00 00 B1 01 E8 A8 F9 37 FF 0F B6 D8 88 44 24 40 33 D2 48 8B CF
	Offset_DxRenderer = 0x1439E75F8, //48 8B 0D ?? ?? ?? ?? 48 85 C9 74 ?? E8 ?? ?? ?? ?? 48 8B 88
	Offset_GameRenderView = 0x143803330,
	Offset_BorderInput = 0x143492de8,
	Offset_CameraMgr = 0x143803328
};

enum class EVariableOffsets : size_t
{
	Offset_HWND = 0x1435315C0,
	Offset_SetupDrawCall = 0x0140F93D20
};

enum class EFunctionOffsets : size_t
{
	Offset_SMChams = 0, // 48 89 d0 0f 28 00 66 41 0f 7f 01 f6 42 40 02 74
	Offset_ViewAngles = 0x1413E7750, //41 0F 28 C8 41 0F 57 CB F3 0F 59 C7 0F 28 D0 41 0F 57 D3 0F 2F D6 77 03 0F 28 D6 41 0F 2F CA 77 04 41 0F 28 CA 0F 2F C2 72 03 0F 28 C2 44 0F 2F C1 72 04 44 0F 28 C1 F3 44 0F 11 85 40 01 00 00 F3 0F 11 85 44 01 00 00 48 8B 85 40 01 00 00 48 89 85 48 01 00 00 45 0F 57 F6 F3 0F 10 B5 4C 01 00 00 F3 44 0F 10 95 48 01 00 00 F3 0F 11 B5 40 01 00 00 F3 44 0F 11 95 48 01 00 00 F3 41 0F 10 07 F3 41 0F 10 4E 48 F3 44 0F 10 0D 18 7D 5E 01 41 BD 02 00 00 00 F3 41 0F 59 47 24 F3 0F 5C C8 F3 0F 11 4C 24 38 F3 41 0F 11 4E 48 8B 44 24 38 41 33 C6 41 89 86 C4 00 00 00 49 8B 87 D0 00 00 00 48 85 C0 0F 84 32 06 00 00 45 0F 2E B7 D8 00 00 00 75 0E 45 0F 2E B7 DC 00 00 00 0F 84 1A 06 00 00 4C 8B 48 08 0F B6 40 18 41 0F B6 49 0A 48 C1 E0 05 42 0F B6 54 08 14 80 FA FF 0F 84 5F 01 00 00 0F B6 C2 41 8B D4 4D 8D 41 50 48 03 C8 41 0F B6 41 09 48 8D 0C 48 41 8B C4 48 C1 E1 05 4C 03 C1 41 80 79 0B 6C 0F 94 C2 41 80 79 0B 37 4C 89 44 24 30 0F 94 C0 0B D0
	Offset_TwinkleCrash = 0x14461A518,
	Offset_NoBreath = 0x14140FBD2,
	Offset_NoRecoil = 0x14729FD14,
	Offset_NoSpread = 0x1473Eb2F9,
	Offset_Radar = 0x1479D815A,
	Offset_SpottingRange = 0x141696F85,
	Offset_SpottingAlways = 0x1415E6BC3,
	Offset_SpottingFoV = 0x141696F85,
	Offset_SpottingVisibility = 0x1415EA71A,
	Offset_SpottingOccluded = 0x147DFBF00,
	Offset_SpottingTeam = 0x1415E7620,
	Offset_SetViewParameters = 0x140D00AD0,
	Offset_InitializeEntityRenderPipeline = 0x140F5FDF0,
	Offset_InitializeVehicleRenderPipeline = 0x140F5DA40,
	Offset_PrepareVehicleRenderer = 0x140BF4850,
	Offset_PrepareEntityRenderer = 0x146D58810,
	Offset_AllocateRenderFunc = 0x140329610,
	Offset_PBRLight = 0x146D18970,
	Offset_DispatchFunctions = 0x1446D5110,
	Offset_CullTerrain = 0x146CE65E7,
	Offset_Cull = 0x1410D6337,
	Offset_CullCheck = 0x1410D6734,
	Offset_RenderMerge = 0x140F66D90,
	Offset_CalculateShootspaceTargetMatrix = 0x1446CEFC4,
	Offset_Spot = 0x1415E6A20,
	Offset_AlwaysSprint = 0x0,
	Offset_BipodEverywhere = 0x141829c84, // 0x90 0x90
	Offset_BipodWhileMoving = 0x141829C72, // 6 0x90
	Offset_BipodEveryWeapon = 0x141829cbb, // 0x90 0x901
	Offset_CreateCamera = 0x140BAC600,
	Offset_ThirdPersonBody = 0x146072B9B,
	Offset_ThirdPersonCameraTransform = 0x1413FA033,
	Offset_BodyTransform = 0x14678E764,
	Offset_UpdateShootspace = 0x1413EEB6F,
	Offset_LookAt = 0x1446CEFC0,
	Offset_LookAtRay = 0x1413E0392,
	Offset_CrosshairWhenZoomedIn = 0x14181E0D4
};

enum class EComponentClassOffsets : size_t
{
	Offset_SpottingComponent = 0x143EAC690
};

enum class EVMTOffsets : size_t
{
	Offset_CSE_VMT = 0x142C67CD8,
	Offset_RVP_VMT = 0x142B09978,
	Offset_BLT_VMT = 0x142CD7790,
	Offset_INP_VMT = 0x142AFD8E8,
	Offset_SLD_VMT = 0x142BD5990,
	Offset_RCE_VMT = 0x142C7F148,
	Offset_WPN_VMT = 0x142D689B8,
	Offset_PHS_VMT = 0x142C67B10,
	Offset_AIM_VMT = 0x142C682A0
};

enum class EVMTFunctionsWeapon : size_t
{
	Index_SetShootspace = 0x1B
};

enum class EVMTFunctionsSoldierPhysics
{
	Index_ViewOffset = 0x08
};

enum EVMTFunctionsClientPlayer : size_t
{
	Index_UpdatePosition = 0x76,
	Index_GetComponent = 0x23
};

enum EVMTFunctionsRenderViewParams : size_t
{
	Index_GetView = 0x06
};

enum class EVMTFunctionsProjectile : size_t
{
	Index_UpdatePosition = 0x4A
};

enum class EVMTFunctionsInput : size_t
{
	Index_GetInput = 0x08
};

enum class EVMTFunctionsSwapchain : size_t
{
	Index_Present = 0x01
};

enum class EVMTFunctionsSoldierRenderer : size_t
{
	Index_SetBuffers = 0x01
};

enum class EVMTFunctionsSoldierEntityRenderer : size_t
{
	Index_RenderPrimitive = 0x0F
};

enum class ERenderPassOffsets : size_t
{
	Offset_RenderPassFg = 0x142C0FE20,
	Offset_RenderPassMain = 0x142C0FE08
};

enum class EVMTAiming : size_t
{
	Offset_GetYaw = 0x11,
	Offset_GetPitch = 0x12
};