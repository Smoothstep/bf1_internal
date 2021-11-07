#pragma once

#include "Math.h"

namespace Hyper
{
	template<class PointType> 
	struct Point
	{
	public:

		PointType X;
		PointType Y;

	public:

		FORCEINLINE constexpr Point() = default;
		FORCEINLINE constexpr Point
		(
			IN PointType X,
			IN PointType Y
		) : X(X), Y(Y)
		{}

		FORCEINLINE constexpr PointType Min() const
		{
			return X > Y ? Y : X;
		}

		FORCEINLINE constexpr PointType Max() const
		{
			return X < Y ? Y : X;
		}

		FORCEINLINE constexpr PointType Product() const
		{
			return X * Y;
		}

		FORCEINLINE constexpr bool operator==
		(
			const Point Other
		)	const
		{
			return 
				Other.X == X && 
				Other.Y == Y;
		}

		FORCEINLINE constexpr bool operator!=
		(
			const Point Other
		)	const
		{
			return 
				Other.X != X ||
				Other.Y != Y;
		}

		FORCEINLINE constexpr bool operator<=
		(
			const Point Other	
		)	const
		{
			return
				X <= Other.X &&
				Y <= Other.Y;
		}

		FORCEINLINE constexpr Point operator /
		(
			const Point Value
		)	const
		{
			Point Result(*this);
			Result.X /= Value.X;
			Result.Y /= Value.Y;
			return Result;
		}

		FORCEINLINE constexpr Point operator /
		(
			const PointType Value
		)	const
		{
			Point Result(*this);
			Result.X /= Value;
			Result.Y /= Value;
			return Result;
		}

		FORCEINLINE constexpr Point operator *
		(
			const PointType Value
		)	const
		{
			Point Result(*this);
			Result.X *= Value;
			Result.Y *= Value;
			return Result;
		}

		FORCEINLINE constexpr Point & operator /=
		(
			const PointType Value
		)
		{
			X /= Value;
			Y /= Value;
			return *this;
		}

		FORCEINLINE constexpr Point & operator *=
		(
			const PointType Value
		)
		{
			X *= Value;
			Y *= Value;
			return *this;
		}

		FORCEINLINE constexpr Point & operator +=
		(
			const PointType Value
		)
		{
			X += Value;
			Y += Value;
			return *this;
		}

		FORCEINLINE constexpr Point & operator +=
		(
			const Point P
		)
		{
			X += P.X;
			Y += P.Y;
			return *this;
		}

		FORCEINLINE constexpr Point & operator -=
		(
			const PointType Value
		)
		{
			X -= Value;
			Y -= Value;
			return *this;
		}

		FORCEINLINE constexpr Point & operator -=
		(
			const Point P
		)
		{
			X -= P.X;
			Y -= P.Y;
			return *this;
		}

		FORCEINLINE constexpr Point operator +
		(
			const PointType Value
		)	const
		{
			Point Result(*this);
			Result.X += Value;
			Result.Y += Value;
			return Result;
		}

		FORCEINLINE constexpr Point operator +
		(
			const Point Value
		)	const
		{
			Point Result(*this);
			Result.X += Value.X;
			Result.Y += Value.Y;
			return Result;
		}

		FORCEINLINE constexpr Point operator -
		(
			const PointType Value
		)	const
		{
			Point Result(*this);
			Result.X -= Value;
			Result.Y -= Value;
			return Result;
		}

		FORCEINLINE constexpr Point operator -
		(
			const Point Value
		)	const
		{
			Point Result(*this);
			Result.X -= Value.X;
			Result.Y -= Value.Y;
			return Result;
		}
	};

	using IntPoint		= Point<int>;
	using LongPoint		= Point<long>;
	using UintPoint		= Point<unsigned int>;
	using UlongPoint	= Point<unsigned long>;
}