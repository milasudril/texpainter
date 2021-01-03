//@	{"targets":[{"name":"size_2d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_SIZE2D_HPP
#define TEXPAINTER_UTILS_SIZE2D_HPP

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include <cstdint>
#include <limits>
#include <string>
#include <cmath>

namespace Texpainter
{
	class Size2d
	{
	public:
		constexpr explicit Size2d(uint32_t width, uint32_t height): m_width{width}, m_height{height}
		{
		}

		constexpr auto width() const { return m_width; }

		constexpr auto height() const { return m_height; }

	private:
		uint32_t m_width;
		uint32_t m_height;
	};

	constexpr inline bool operator==(Size2d a, Size2d b)
	{
		return a.width() == b.width() && a.height() == b.height();
	}

	constexpr inline bool operator!=(Size2d a, Size2d b) { return !(a == b); }

	constexpr inline auto area(Size2d size)
	{
		return static_cast<size_t>(size.width()) * static_cast<size_t>(size.height());
	}

	constexpr inline auto aspectRatio(Size2d size)
	{
		return static_cast<double>(size.width()) / static_cast<double>(size.height());
	}

	constexpr inline auto toDiameter(Size2d size, double exponent)
	{
		return std::max(2.0, std::sqrt(area(size)) * std::exp2(std::lerp(-16.0, 0.0, exponent)));
	}

	template<class T>
	bool isSupported(Size2d size)
	{
		auto const a = area(size);
		return a != 0 && a < std::numeric_limits<size_t>::max() / sizeof(T);
	}

	inline std::string toString(Size2d size)
	{
		return std::to_string(size.width()) + " × " + std::to_string(size.height());
	}

	inline void to_json(nlohmann::json& j, Size2d size)
	{
		j = nlohmann::json{std::pair{"width", size.width()}, std::pair{"height", size.height()}};
	}


}

#endif