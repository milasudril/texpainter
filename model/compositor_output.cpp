//@	{
//@	  "targets":[{"name":"compositor_output.o", "type":"object"}]
//@	}

#include "./compositor_output.hpp"

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
	std::unique_ptr<T[]> downsample(Texpainter::Size2d size, T const* src, uint32_t scale)
	{
		if(scale == 1) [[likely]]
			{
				auto ret = std::make_unique<T[]>(area(size));
				std::copy_n(src, area(size), ret.get());
				return ret;
			}

		auto const w = size.width() / scale;
		auto const h = size.height() / scale;

		auto ret = std::make_unique<T[]>(w * h);

		for(uint32_t row = 0; row < h; ++row)
		{
			for(uint32_t col = 0; col < w; ++col)
			{
				T result{};
				for(uint32_t row_src = 0; row_src < scale; ++row_src)
				{
					for(uint32_t col_src = 0; col_src < scale; ++col_src)
					{
						auto const x = scale * col + col_src;
						auto const y = scale * row + row_src;
						result += src[y * size.width() + x];
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
	    Texpainter::Size2d size, Texpainter::FilterGraph::PortValue const& src, uint32_t resolution)
	{
		return visit(
		    [size, resolution](auto const& item) {
			    return Texpainter::FilterGraph::PortValue{downsample(
			        size, Texpainter::FilterGraph::makeInputPortValue(item), resolution)};
		    },
		    src);
	}
}

Texpainter::Model::CompositorOutput::CompositorOutput(Size2d size,
                                                      FilterGraph::PortValue const& src,
                                                      uint32_t resolution)
    : m_size{size.width() / resolution, size.height() / resolution}
    , m_data{downsample_impl(size, src, resolution)}
{
}