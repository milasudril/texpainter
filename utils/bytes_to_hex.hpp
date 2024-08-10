//@	{"targets":[{"name":"bytes_to_hex.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_BYTESTOHEX_HPP
#define TEXPAINTER_UTILS_BYTESTOHEX_HPP

#include <algorithm>
#include <string>
#include <span>
#include <cstdint>

namespace Texpainter
{
	namespace detail
	{
		constexpr char const* digits = "0123456789abcdef";

		constexpr auto digit_to_number(char val)
		{
			return val <= '9' ? val - '0' : (val & '_') - 'A' + 0xa;
		}

		constexpr auto out_of_range(int val) { return val > 15 || val < 0; }
	};

	constexpr void bytesToHex(std::span<std::byte const> src, char* output)
	{
		std::ranges::for_each(src, [output](std::byte val) mutable {
			auto const msb = (static_cast<uint8_t>(val) & 0xf0) >> 4;
			auto const lsb = (static_cast<uint8_t>(val) & 0x0f);

			*output       = detail::digits[msb];
			*(output + 1) = detail::digits[lsb];
			output += 2;
		});
	}

	constexpr bool hexToBytes(std::string_view src, std::byte* output)
	{
		if(std::size(src) % 2 != 0) { return false; }

		auto ptr = std::begin(src);
		while(ptr != std::end(src))
		{
			auto const msb = detail::digit_to_number(*ptr);
			auto const lsb = detail::digit_to_number(*(ptr + 1));

			if(detail::out_of_range(msb) || detail::out_of_range(lsb)) { return false; }

			*output = static_cast<std::byte>((msb << 4) | lsb);

			ptr += 2;
			++output;
		}
		return true;
	}

	inline std::string bytesToHex(std::span<std::byte const> src)
	{
		std::string ret;
		ret.resize(2 * std::size(src));
		bytesToHex(src, std::data(ret));
		return ret;
	}
}

#endif