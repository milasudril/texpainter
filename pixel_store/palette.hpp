//@	{"targets":[{"name":"palette.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PIXELSTORE_PALETTE_HPP
#define TEXPAINTER_PIXELSTORE_PALETTE_HPP

#include "./pixel.hpp"
#include "./color_index.hpp"

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>

namespace Texpainter::PixelStore
{
	template<ColorIndex::element_type Size>
	class Palette
	{
	public:
		using index_type = ColorIndex;

		constexpr auto begin() const { return std::begin(m_data); }

		constexpr auto end() const { return std::end(m_data); }

		constexpr auto begin() { return std::begin(m_data); }

		constexpr auto end() { return std::end(m_data); }

		constexpr auto rbegin() const { return std::rbegin(m_data); }

		constexpr auto rend() const { return std::rend(m_data); }

		constexpr auto rbegin() { return std::rbegin(m_data); }

		constexpr auto rend() { return std::rend(m_data); }

		static constexpr auto size() { return Size; }


		constexpr explicit Palette(Pixel color_init = Pixel{0.0f, 0.0f, 0.0f, 0.0f})
		{
			std::ranges::fill(m_data, color_init);
		}

		constexpr auto operator[](index_type index) const { return *(begin() + index.value()); }

		constexpr auto& operator[](index_type index)
		{
			return *(std::begin(m_data) + index.value());
		}

		constexpr auto lastIndex() const { return index_type{size() - 1}; }

	private:
		std::array<Pixel, Size> m_data;
	};

	template<ColorIndex::element_type Size>
	void to_json(nlohmann::json& j, Palette<Size> const& pal)
	{
		std::array<std::string, static_cast<size_t>(Size)> colors;
		std::ranges::transform(
		    pal, std::begin(colors), [](auto const& val) { return toString(val); });
		j = nlohmann::json{std::pair{"colorspace", "linear"}, std::pair{"colors", colors}};
	}

	template<ColorIndex::element_type Size>
	void store(Palette<Size> const& pal, char const* filename)
	{
		nlohmann::json obj;
		to_json(obj, pal);
		auto const str = obj.dump(1, '\t');

		auto const f = fopen(filename, "wb");
		if(f == nullptr)
		{ throw std::string{"Failed to open "} + filename + ": " + strerror(errno); }
		fputs(str.c_str(), f);
		fclose(f);
	}

	template<ColorIndex::element_type Size>
	void from_json(nlohmann::json const& j, Palette<Size>& pal)
	{
		auto colorspace = j.at("colorspace").get<std::string>();
		auto colors     = j.at("colors").get<std::array<std::string, static_cast<size_t>(Size)>>();

		if(colorspace == "g22") { throw std::string{"Unsupported colorspace "} + colorspace; }
		else if(colorspace == "linear")
		{
			std::ranges::transform(colors, std::begin(pal), [](auto const& value) {
				return fromString(Enum::Empty<Pixel>{}, value);
			});
		}
		else
		{
			throw std::string{"Unsupported colorspace "} + colorspace;
		}
	}

	template<ColorIndex::element_type Size>
	Palette<Size> load(char const* filename)
	{
		auto const f = std::unique_ptr<FILE, int (*)(FILE*)>{fopen(filename, "rb"), fclose};
		if(f == nullptr)
		{ throw std::string{"Failed to open "} + filename + ": " + strerror(errno); }

		auto obj = nlohmann::json::parse(f.get());
		Palette<Size> ret;
		from_json(obj, ret);
		return ret;
	}
}

#endif