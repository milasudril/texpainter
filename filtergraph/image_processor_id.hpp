//@	{
//@	 "targets":[{"name":"image_processor_id.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"image_processor_id.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORID_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORID_HPP

#include "utils/bytes_to_hex.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <cstdint>
#include <algorithm>
#include <string>

namespace Texpainter::FilterGraph
{
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
			if(!hexToBytes(id, std::data(m_data)))
			{ throw "String should be a 128 bit number written in hexadecimal notation"; }
		}

		explicit ImageProcessorId(std::string_view str)
		{
			if(std::size(str) != 32) { throw "A ImageProcessorId must be 32 bytes"; }

			if(!hexToBytes(str, std::data(m_data)))
			{ throw "String should be a 128 bit number written in hexadecimal notation"; }
		}

		constexpr auto const& data() const { return m_data; }

	private:
		std::array<std::byte, 16> m_data;
	};

	constexpr auto InvalidImgProcId = ImageProcessorId{'0'};

	constexpr bool operator==(ImageProcessorId a, ImageProcessorId b)
	{
		return a.data() == b.data();
	}

	constexpr bool operator!=(ImageProcessorId a, ImageProcessorId b) { return !(a == b); }

	std::string toString(ImageProcessorId const& id);

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

	void to_json(nlohmann::json& obj, ImgProcReleaseState val);

	void from_json(nlohmann::json const& obj, ImgProcReleaseState& val);


}

#endif