//@	{
//@	  "targets":[{"name":"compositor_output.o", "type":"object"}]
//@	}

#include "./compositor_output.hpp"

#include "utils/span_2d.hpp"
#include "pixel_store/image_io.hpp"

namespace
{
	template<class T>
	struct ScalarType
	{
		using type = T;
	};

	template<>
	struct ScalarType<Texpainter::PixelStore::Pixel>
	{
		using type = float;
	};

	template<class T>
	struct ScalarType<std::complex<T>>
	{
		using type = T;
	};

	template<class T>
	std::unique_ptr<T[]> downsample(Texpainter::Size2d size, T const* src, uint32_t scale)
	{
		auto ret = std::make_unique<T[]>(area(size));

		if(scale == 1) [[likely]]
			{
				std::copy_n(src, area(size), ret.get());
				return ret;
			}

		auto const w = size.width();
		auto const h = size.height();

		for(uint32_t row = 0; row != h; ++row)
		{
			for(uint32_t col = 0; col != w; ++col)
			{
				T result{};
				for(uint32_t row_src = 0; row_src != scale; ++row_src)
				{
					for(uint32_t col_src = 0; col_src != scale; ++col_src)
					{
						auto const x = scale * col + col_src;
						auto const y = scale * row + row_src;
						result += src[y * scale * size.width() + x];
					}
				}
				ret[row * w + col] =
				    result / static_cast<typename ScalarType<T>::type>(scale * scale);
			}
		}

		return ret;
	}

	template<class T>
	T downsample(Texpainter::Size2d, T const& val, uint32_t)
	{
		return val;
	}

	template<class T>
	T downsample(Texpainter::Size2d size, std::reference_wrapper<T const> val, uint32_t scale)
	{
		return downsample(size, val.get(), scale);
	}

	Texpainter::FilterGraph::PortValue downsample_impl(
	    Texpainter::Size2d size, Texpainter::FilterGraph::PortValue const& src, uint32_t scale)
	{
		return visit(
		    [size, scale](auto const& item) {
			    return Texpainter::FilterGraph::PortValue{
			        downsample(size, Texpainter::FilterGraph::makeInputPortValue(item), scale)};
		    },
		    src);
	}

	template<class T>
	void store(T const&, Texpainter::Size2d, char const*)
	{
	}

	void store(std::unique_ptr<Texpainter::PixelStore::Pixel[]> const& src,
	           Texpainter::Size2d size,
	           char const* filename)
	{
		store(Texpainter::Span2d{src.get(), size}, filename);
	}

	template<class T>
	void store_impl(T const& src, Texpainter::Size2d size, char const* filename)
	{
		store(src, size, filename);
	}
}

Texpainter::Model::CompositorOutput::CompositorOutput(Size2d size,
                                                      FilterGraph::PortValue const& src,
                                                      uint32_t scale)
    : m_size{size}
    , m_data{downsample_impl(size, src, scale)}
{
}

void Texpainter::Model::store(CompositorOutput const& src, char const* filename)
{
	visit([size = src.size(), filename](auto const& item) { store_impl(item, size, filename); },
	      src.data());
}