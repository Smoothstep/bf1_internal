#pragma once

#include "Math.h"

namespace Hyper
{
	class Float32
	{
	public:

		union
		{
			struct
			{
				uint32	Sign : 1;
				uint32	Exponent : 8;
				uint32	Mantissa : 23;
			} Components;

			float	FloatValue;
		};

		Float32(float InValue = 0.0f);
	};


	FORCEINLINE Float32::Float32(float InValue)
		: FloatValue(InValue)
	{ }
}