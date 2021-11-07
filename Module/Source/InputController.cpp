#include "stdafx.h"
#include "InputController.h"
#include "Hooks.h"

float InputController::InputCache::GetInput(InputType Type)
{
	auto HkRef = HookRefSpec<InputController&>::Current();
	auto& [Ic] = HkRef.Tuple();
	const float InputValue = (this->*HkRef->GetOriginal<decltype(&InputController::InputCache::GetInput)>())(Type);
	return Ic.Inputs[Type] = InputValue + Ic.InputsOverriden[Type].exchange(0.f);
}

InputController::InputAngle::InputAngle()
{
	Bitconvert<int, float> Conv;
	Conv.From = -1;
	Current = Conv.To;
	Last = Conv.To;
}

constexpr float InputController::InputAngle::operator=(const float Value)
{
	return Current = Last + Value;
}

constexpr InputController::InputAngle::operator float() const
{
	return Last;
}

void InputController::ApplyViewParameters(Vector2f Angles)
{
	if (ViewOffset)
	{
		Angles /= 5.0f;
	}

	switch (GetInputType())
	{
	case EInputType::TypeDirect:
		InputAngles[0] = Angles.X;
		InputAngles[1] = Angles.Y;
		break;

	case EInputType::TypeRaw:
		if (Angles.X < 0)
		{
			InputsOverriden[InputCache::Input_YawL] = -Angles.X;
		}
		else
		{
			InputsOverriden[InputCache::Input_YawR] = +Angles.X;
		}

		if (Angles.Y < 0)
		{
			InputsOverriden[InputCache::Input_PitchU] = -Angles.Y;
		}
		else
		{
			InputsOverriden[InputCache::Input_PitchD] = +Angles.Y;
		}
		break;

	default:
		break;
	}
}

InputController::InputController()
	: InputAngles{}
	, InputType{ EInputType::TypeRaw }
	, Inputs{}
	, InputsOverriden{}
	, LookAtTarget{}
	, ViewOffset{}
{}

bool InputController::IsFiring() const
{
	return Inputs[InputCache::Input_Fire] > 0.0f;
}

bool InputController::HasInputsPending() const
{
	for (const auto& Input : InputsOverriden)
	{
		if (Input != 0.0f)
		{
			return true;
		}
	}

	return false;
}

void InputController::SetFireState(bool Fire)
{
	InputsOverriden[InputCache::Input_Fire] = Fire ? 1.0f : 0.0f;
}

EInputType InputController::GetInputType() const
{
	return ViewOffset ? EInputType::TypeDirect : InputType;
}

void InputController::SetThirdPersonMode(bool On)
{
	constexpr Vector3f ThirdPersonOffset = Vector3f{ 0.0f, 3.5f, -3.0f };

	if (On)
	{
		ViewOffset = std::make_optional(ThirdPersonOffset);
	}
	else
	{
		ViewOffset = {};
	}
}

void InputController::SetInputType(EInputType Type)
{
	InputType = Type;
}

void InputController::SetLookAt(std::optional<Vector3f> Target)
{
	LookAtTarget = Target;
}

std::optional<Vector3f> InputController::GetLookAt()
{
	return LookAtTarget;
}

std::optional<Vector3f> InputController::GetViewOffset() const
{
	return ViewOffset;
}