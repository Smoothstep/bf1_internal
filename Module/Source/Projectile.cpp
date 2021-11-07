#include "stdafx.h"
#include "Hooks.h"
#include "Game.h"
#include "Projectile.h"

static Hyper::Matrix4x4 TrackedMatrix = Hyper::Matrix4x4::Identity();
static Vector3f TargetPosition;
static bool IsTracking = false;

RenderViewLayout* RenderViewParams::CustomLayout(uint32_t Arg0, uint32_t Arg1)
{
	RenderViewLayout* View = ((*this).*HookRef::Current()->GetOriginal<decltype(&RenderViewParams::CustomLayout)>())(Arg0, Arg1);

	if (IsTracking)
	{
		View->Vpm = TrackedMatrix;
	}

	return View;
}

void RenderViewParams::StartTracking(const Vector3f& Target)
{
	const auto OriginBefore = TrackedMatrix.GetOrigin();
	const auto Origin = Target;
	const auto Direction = (Origin - TargetPosition).GetSafeNormal();

	const Vector3f Up = Vector3f::UpVector;
	const Vector3f XAxis = Vector3f::CrossProduct(Up, Direction).GetSafeNormal();
	const Vector3f YAxis = Vector3f::CrossProduct(Direction, XAxis).GetSafeNormal();

	Matrix4x4 Mat = Matrix4x4::Identity();

	Mat[0][0] = XAxis.X;
	Mat[0][1] = XAxis.Y;
	Mat[0][2] = XAxis.Z;

	Mat[1][0] = YAxis.X;
	Mat[1][1] = YAxis.Y;
	Mat[1][2] = YAxis.Z;

	Mat[2][0] = Direction.X;
	Mat[2][1] = Direction.Y;
	Mat[2][2] = Direction.Z;

	TrackedMatrix = Mat.WithTranslation(Origin);
	IsTracking = true;
}


void RenderViewParams::SetViewMatrix(const Matrix4x4& Mat)
{
	IsTracking = true;
	TrackedMatrix = Mat;
}

void RenderViewParams::SetLookAt(const Vector3f& Position)
{
	TargetPosition = Position;
}

void RenderViewParams::StopTracking()
{
	IsTracking = false;
}

void RenderViewLayout::CopyFrom(void* Data)
{
	memcpy(&Transform, Data, 0x90);

	if (IsTracking)
	{
		Vpm = TrackedMatrix;
	}
}


bool Projectile::ComputePosition(void* Arg0, void* Arg1)
{
	bool Result = ((this)->*HookRef::Current()->GetOriginal<decltype(&Projectile::ComputePosition)>())(Arg0, Arg1);

#ifdef TRACK_PROJECTILE
	if (Result)
	{
		RenderViewParams::StartTracking(this);
	}
	else
	{
		RenderViewParams::StopTracking();
	}
#endif

	return Result;
}