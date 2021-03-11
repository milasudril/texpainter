//@	{
//@	 "targets":[{"name":"image_processor_id.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORID_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORID_HPP

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include <array>
#include <cstdint>
#include <algorithm>
#include <string>

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		constexpr uint8_t from_hex_digit(char x)
		{
			if(x < 0) { throw "String should be a 128 bit number written in hexadecimal notation"; }
			x = x >= 'A' + 32 ? x - 32 : x;

			if((x > '9' && x < 'A') || x > 'F')
			{ throw "String should be a 128 bit number written in hexadecimal notation"; }

			return static_cast<std::uint8_t>((x >= '0' && x <= '9') ? x - '0' : 10 + (x - 'A'));
		}

		constexpr char to_hex_digit(std::byte val)
		{
			auto i = static_cast<uint8_t>(val);
			if(i < 10) { return static_cast<char>(i + '0'); }
			return static_cast<char>((i - 10) + 'a');
		}

		constexpr std::pair<char, char> to_hex_digits(std::byte val)
		{
			return std::make_pair(to_hex_digit(val >> 4),
			                      to_hex_digit(val & static_cast<std::byte>(0x0f)));
		}
	}

	class ImageProcessorId
	{
	public:
		ImageProcessorId() = default;

		consteval explicit ImageProcessorId(char const val)
		{
			if(val != '0') { throw "Blah"; }
			std::ranges::fill(m_data, static_cast<std::byte>(0));
		}

		template<size_t N>
		constexpr explicit ImageProcessorId(char const (&id)[N])
		{
			static_assert(N == 33,  // Remember nul terminator
			              "String should be a 128 bit number written in hexadecimal notation");
			set_val(id);
		}

		explicit ImageProcessorId(std::string_view str)
		{
			if(std::size(str) != 32) { throw "A ImageProcessorId must be 32 bytes"; }

			set_val(std::data(str));
		}

		constexpr auto const& data() const { return m_data; }

	private:
		std::array<std::byte, 16> m_data;

		constexpr void set_val(char const* id)
		{
			auto ptr = std::begin(m_data);
			uint8_t msb{};
			for(int k = 0; k < 32; ++k)
			{
				if(k % 2 == 0) { msb = detail::from_hex_digit(id[k]); }
				else
				{
					auto const lsb = detail::from_hex_digit(id[k]);
					*ptr           = static_cast<std::byte>((msb << 4) | lsb);
					++ptr;
				}
			}
		}
	};

	constexpr auto InvalidImgProcId = ImageProcessorId{'0'};

	constexpr bool operator==(ImageProcessorId a, ImageProcessorId b)
	{
		return a.data() == b.data();
	}

	constexpr bool operator!=(ImageProcessorId a, ImageProcessorId b) { return !(a == b); }

	inline std::string toString(ImageProcessorId const& id)
	{
		std::string ret{};
		ret.reserve(32);
		std::ranges::for_each(id.data(), [&ret](auto val) {
			auto hexdigits = detail::to_hex_digits(val);
			ret.push_back(hexdigits.first);
			ret.push_back(hexdigits.second);
		});
		return ret;
	}

	inline void to_json(nlohmann::json& obj, ImageProcessorId const& id) { obj = toString(id); }

	inline void from_json(nlohmann::json const& obj, ImageProcessorId& id)
	{
		id = ImageProcessorId{obj.get<std::string>()};
	}

	enum class ImgProcReleaseState : int
	{
		Experimental,
		Stable,
		Deprecated
	};
}

#endif