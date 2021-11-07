#pragma once

#include "Hyper.h"

class InputController;
class Utility;

class ClientPlayer;
class ClientSoldierEntity;
class ClientVehicleEntity;
class ClientPlayerManager;
class ClientGameContext;
class PhysicsEntityBase;
class GameRenderer;
class GameLevel;

enum EBones : size_t
{
	BoneHead = 0x35,
	BoneNeck = 0x33,
	BoneSpine2 = 7,
	BoneSpine1 = 6,
	BoneSpine = 5,
	BoneLeftShoulder = 8,
	BoneRightShoulder = 0xa3,
	BoneLeftElbowRoll = 14,
	BoneRightElbowRoll = 0xa9,
	BoneLeftHand = 0x10,
	BoneRightHand = 0xab,
	BoneLeftKneeRoll = 0x11f,
	BoneRightKneeRoll = 0x12d,
	BoneLeftFoot = 0x115,
	BoneRightFoot = 0x123,
	BoneAny = ~0ULL
};

enum ETraceActorFlags
{
	CheckMesh = 1 << 0,
	AsyncRaycast = 1 << 1,
	NoWater = 1 << 2,
	NoTerrain = 1 << 3,
	NoRagdoll = 1 << 4,
	NoCharacter = 1 << 5,
	NoGroup = 1 << 6,
	NoPhantoms = 1 << 7,
	NoCheck = NoWater | NoRagdoll | NoCharacter | NoPhantoms,
};

enum class EScreenshotPolicy
{
	DoNothing,
	PolicyIgnore,
	PolicyClean,
	PolicyBlacken
};

enum class EInputType
{
	TypeDirect,
	TypeRaw
};

struct MaterialInfo
{
	enum MaterialFlag
	{
		SeeThrough = 1 << 0,
		Penetrable = 1 << 3,
		ClientDestructable = 1 << 30,
	};

	bool CanPenetrate()
	{
		return (Flags & Penetrable);
	}

	bool CanSeeThrough()
	{
		return (Flags & SeeThrough);
	}

	unsigned long Flags;
};

struct AxisAlignedBoundingBox
{
	Vector4f Min;
	Vector4f Max;
};

struct AABBTransform
{
	Matrix4x4 Transform;
	AxisAlignedBoundingBox AABB;
	Matrix4x4 Offset;
};

enum EMaterials : long
{
	MATERIAL_UNKNOWN
};

enum EPhysicsIds : long
{
	PHYSICS_UNKNOWN
};

struct Default {};
constexpr static Default GDefault;
constexpr size_t MaxPlayers = 70;

namespace D3D
{
	class RenderPass;
	class ViewState;
	class GameInterface;
	class Renderer;
	class RenderIndicators;
	class RenderBones;
	template<size_t>
	class XRayDepthRenderer;
}

template<class T>
class Shader;

enum Alliance : size_t
{
	Alliance_Friendly,
	Alliance_Enemy,
	//Alliance_Neutral,
	Alliance_Num
};

enum VisibilityState
{
	VisDefault = 0xAF,
	VisOutline = 0xF1,
	VisOutlineAlpha = 0xF2,
};

namespace Cef
{
	class UI;
}

using namespace Cef;