#pragma once

#include "ClientPlayer.h"

struct SpottingBehavior
{
	SpottingBehavior() noexcept = default;
	constexpr SpottingBehavior(const Default& Def) noexcept
		: Delay{ 0.5f }
	{}
	
	float Delay;
};

class AutoSpot
{
public:
	AutoSpot();
	~AutoSpot();

	void Execute(const SpottingBehavior& Behavior);

private:
	std::unique_ptr<struct SpottingDetail> Detail;
};