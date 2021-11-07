#pragma once

#include "Lock.h"
#include "ClassDecl.h"
#include <atomic>

class InputController
{
public:
	class InputAngle
	{
	public:
		InputAngle();

		constexpr float operator=(const float Value);
		constexpr operator float() const;

	private:
		float Current;
		float Last;
	};

	class InputCache
	{
	public:
		enum InputType
		{
			Input_YawR,
			Input_YawL,
			Input_PitchU,
			Input_PitchD,
			Input_Fire = 6,
			NumInputTypes = 9
		};

	public:
		float GetInput(InputType Type);
	};

public:
	InputController();

	bool IsFiring() const;
	bool HasInputsPending() const;
	
	void SetFireState(bool Fire);
	void ApplyViewParameters(Vector2f Angles);

	EInputType GetInputType() const;

	void SetThirdPersonMode(bool On);
	void SetInputType(EInputType Type);
	void SetLookAt(std::optional<Vector3f> Target);
	
	std::optional<Vector3f> GetLookAt();
	std::optional<Vector3f> GetViewOffset() const;

protected:
	InputAngle InputAngles[2];
	EInputType InputType;
	std::array<float, InputCache::NumInputTypes> Inputs;
	std::array<std::atomic<float>, InputCache::NumInputTypes> InputsOverriden;
	std::optional<Vector3f> LookAtTarget;
	std::optional<Vector3f> ViewOffset;
};