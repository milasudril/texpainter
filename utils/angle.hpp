//@	{"targets":[{"name":"angle.hpp", "type":"include"}]}

#ifndef TEXPAINTER_ANGLE_HPP
#define TEXPAINTER_ANGLE_HPP

#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>

namespace Texpainter
{
	constexpr auto π = std::numbers::pi;

	class Angle
	{
	public:
		struct Radians
		{
		};
		struct Turns
		{
		};

		constexpr explicit Angle(double α, Radians):
		   m_value{static_cast<uint32_t>(static_cast<int64_t>(0x1p31 * α / π))}
		{
		}

		constexpr explicit Angle(double α, Turns):
		   m_value{static_cast<uint32_t>(static_cast<int64_t>(0x1p32 * α))}
		{
		}

		static constexpr Angle inRadians(double α)
		{
			return Angle{α, Radians{}};
		}

		static constexpr Angle inTurns(double α)
		{
			return Angle{α, Turns{}};
		}

		constexpr double radians() const
		{
			return π * static_cast<int32_t>(m_value) * 0x1p-31;
		}

		constexpr double turns() const
		{
			return static_cast<int32_t>(m_value) * 0x1p-32;
		}

		constexpr int32_t binary() const
		{
			return static_cast<int32_t>(m_value);
		}

		constexpr Angle& operator+=(Angle β)
		{
			m_value += β.bits();
			return *this;
		}

		constexpr Angle operator-=(Angle β)
		{
			m_value -= β.bits();
			return *this;
		}

		constexpr uint32_t bits() const
		{
			return m_value;
		}

	private:
		uint32_t m_value;
	};

	constexpr Angle operator+(Angle α, Angle β)
	{
		return α += β;
	}

	constexpr Angle operator-(Angle α, Angle β)
	{
		return α -= β;
	}

	constexpr bool operator==(Angle α, Angle β)
	{
		return α.bits() == β.bits();
	}

	constexpr bool operator!=(Angle α, Angle β)
	{
		return !(α == β);
	}

	constexpr auto cos(Angle α)
	{
		auto const ret = std::cos(α.radians());
		return std::abs(ret) < std::numeric_limits<double>::epsilon() ? 0.0 : ret;
	}

	constexpr auto sin(Angle α)
	{
		auto const ret = std::sin(α.radians());
		return std::abs(ret) < std::numeric_limits<double>::epsilon() ? 0.0 : ret;
	}

	constexpr auto tan(Angle α)
	{
		return std::tan(α.radians());
	}
}

#endif