#pragma once

#include <Hyper.h>

class Projectile
{
public:
	bool ComputePosition(void*, void*);

private:
	Pad(0x190);
	Vector3f Position;
};