#pragma once

#include "Float32.h"

#include <emmintrin.h>

namespace Hyper
{
	class Float16
	{
	public:
		union
		{
			struct
			{
				uint16	Sign : 1;
				uint16	Exponent : 5;
				uint16	Mantissa : 10;
			} Components;

			uint16	Encoded;
		};

		Float16();

		Float16
		(
			const Float16 & FP16Value
		);

		Float16
		(
			float FP32Value
		);

		Float16
		(
			int16 IntValue
		);

		Float16& operator=
		(
			float FP32Value
		);

		Float16& operator=
		(
			const Float16 & FP16Value
		);

		operator float() const;

		void Set
		(
			float FP32Value
		);

		float GetFloat() const;
	};


	FORCEINLINE Float16::Float16()
		: Encoded(0)
	{ }


	FORCEINLINE Float16::Float16(const Float16& FP16Value)
	{
		Encoded = FP16Value.Encoded;
	}


	FORCEINLINE Float16::Float16(float FP32Value)
	{
		Set(FP32Value);
	}

	FORCEINLINE Float16::Float16(int16 IntValue)
	{
		Set(Float(IntValue));
	}


	FORCEINLINE Float16& Float16::operator=(float FP32Value)
	{
		Set(FP32Value);
		return *this;
	}


	FORCEINLINE Float16& Float16::operator=(const Float16& FP16Value)
	{
		Encoded = FP16Value.Encoded;
		return *this;
	}


	FORCEINLINE Float16::operator float() const
	{
		return GetFloat();
	}


	FORCEINLINE void Float16::Set(float FP32Value)
	{
#ifdef _cvtss_sh
		Encoded = _cvtss_sh(FP32Value, 0);
#endif
	}


	FORCEINLINE float Float16::GetFloat() const
	{
#ifdef _cvtsh_ss
		return _cvtsh_ss(Encoded);
#else
		return 0;
#endif
	}
}