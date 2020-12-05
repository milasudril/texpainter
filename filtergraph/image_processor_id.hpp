//@	{
//@	 "targets":[{"name":"image_processor_id.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORID_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORID_HPP

#include <array>
#include <cstdint>
#include <algorithm>

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		consteval uint8_t from_hex_digit(char x)
		{
			if(x < 0) { throw "String should be a 128 bit number written in hexadecimal notation"; }
			x &= ~0x70;

			if((x > '9' && x < 'A') || x > 'F')
			{ throw "String should be a 128 bit number written in hexadecimal notation"; }

			return static_cast<std::uint8_t>((x >= '0' && x <= '9') ? x - '0' : x - 'A');
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
		consteval explicit ImageProcessorId(char const (&id)[N])
		{
			static_assert(
			    N == 33,
			    "String should be a 128 bit number written in hexadecimal notation");  // Remember nul terminator
			auto ptr = std::begin(m_data);
			uint8_t msb;
			uint8_t lsb;
			for(size_t k = 0; k < 32; ++k)
			{
				if(k % 2 == 0) { msb = detail::from_hex_digit(id[k]); }
				if(k % 2 != 0)
				{
					lsb  = detail::from_hex_digit(id[k]);
					*ptr = static_cast<std::byte>((msb << 4) | lsb);
					++ptr;
				}
			}
		}

		constexpr auto data() const { return m_data; }


	private:
		std::array<std::byte, 16> m_data;
	};

	constexpr auto InvalidImgProcId = ImageProcessorId{'0'};

	constexpr bool operator==(ImageProcessorId a, ImageProcessorId b)
	{
		return a.data() == b.data();
	}

	constexpr bool operator!=(ImageProcessorId a, ImageProcessorId b) { return !(a == b); }
}

#endif