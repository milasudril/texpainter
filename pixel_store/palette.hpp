//@	{"targets":[{"name":"palette.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PIXELSTORE_PALETTE_HPP
#define TEXPAINTER_PIXELSTORE_PALETTE_HPP

#include "./pixel.hpp"
#include "./color_index.hpp"

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include <array>
#include <algorithm>
#include <span>

namespace Texpainter::PixelStore
{
	template<class PixelType, ColorIndex::element_type Size>
	class Palette
	{
	public:
		using index_type = ColorIndex;
		using value_type = PixelType;

		constexpr auto begin() const { return std::begin(m_data); }

		constexpr auto end() const { return std::end(m_data); }

		constexpr auto begin() { return std::begin(m_data); }

		constexpr auto end() { return std::end(m_data); }

		constexpr auto rbegin() const { return std::rbegin(m_data); }

		constexpr auto rend() const { return std::rend(m_data); }

		constexpr auto rbegin() { return std::rbegin(m_data); }

		constexpr auto rend() { return std::rend(m_data); }

		static constexpr auto size() { return Size; }


		constexpr explicit Palette(PixelType color_init = PixelType{0.0f, 0.0f, 0.0f, 0.0f})
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
		std::array<PixelType, Size> m_data;
	};

	template<ColorIndex::element_type Size>
	using RgbaPalette = Palette<Pixel, Size>;

	template<ColorIndex::element_type Size>
	void to_json(nlohmann::json& j, RgbaPalette<Size> const& pal)
	{
		std::array<std::string, static_cast<size_t>(Size)> colors;
		std::ranges::transform(
		    pal, std::begin(colors), [](auto const& val) { return toString(val); });
		j = nlohmann::json{std::pair{"colorspace", "linear"}, std::pair{"colors", colors}};
	}

	template<ColorIndex::element_type Size>
	void store(RgbaPalette<Size> const& pal, char const* filename)
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

	template<ColorIndex::element_type Size, class OutputStream>
	void store(RgbaPalette<Size> const& pal, OutputStream stream)
	{
		nlohmann::json obj;
		to_json(obj, pal);
		auto const str = obj.dump(1, '\t');
		write(stream, std::as_bytes(std::span{str}));
	}

	template<ColorIndex::element_type Size>
	void from_json(nlohmann::json const& j, RgbaPalette<Size>& pal)
	{
		auto colorspace = j.at("colorspace").get<std::string>();
		auto colors     = j.at("colors").get<nlohmann::json::array_t>();

		auto get_converter = [](std::string const& colorspace) {
			if(colorspace == "g22")
			{
				return +[](std::string const& value) {
					return Pixel{
					    fromString(Enum::Empty<BasicPixel<ColorProfiles::Gamma22>>{}, value)};
				};
			}
			if(colorspace == "linear")
			{
				return +[](std::string const& value) {
					return fromString(Enum::Empty<Pixel>{}, value);
				};
			}
			else
			{
				throw std::string{"Unsupported colorspace "} + colorspace;
			}
		};

		std::ranges::transform(colors,
		                       std::begin(pal),
		                       [k         = ColorIndex::element_type{0},
		                        converter = get_converter(colorspace)](auto const& value) mutable {
			                       if(k == Size) { throw std::string{"Palette too large"}; }
			                       ++k;
			                       return converter(value);
		                       });
	}

	template<ColorIndex::element_type Size>
	RgbaPalette<Size> load(Enum::Empty<RgbaPalette<Size>>, char const* filename)
	{
		auto const f = std::unique_ptr<FILE, int (*)(FILE*)>{fopen(filename, "rb"), fclose};
		if(f == nullptr)
		{ throw std::string{"Failed to open "} + filename + ": " + strerror(errno); }

		auto obj = nlohmann::json::parse(f.get());
		RgbaPalette<Size> ret;
		from_json(obj, ret);
		return ret;
	}

	template<ColorIndex::element_type Size, class InputStream>
	RgbaPalette<Size> load(Enum::Empty<RgbaPalette<Size>>, InputStream stream)
	{
		auto const n           = static_cast<size_t>(stream.size());
		auto buffer            = std::make_unique<char[]>(stream.size());
		auto const input_range = std::span{buffer.get(), n};
		read(stream, std::as_writable_bytes(input_range));
		auto obj = nlohmann::json::parse(std::begin(input_range), std::end(input_range));
		RgbaPalette<Size> ret;
		from_json(obj, ret);
		return ret;
	}

	template<ColorIndex::element_type Size>
	bool fileValid(Enum::Empty<RgbaPalette<Size>> e, char const* filename)
	{
		try
		{
			load(e, filename);
			return true;
		}
		catch(...)
		{
			return false;
		}
	}
}

#endif