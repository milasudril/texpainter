//@	{
//@	 "targets":[{"name":"document.o", "type":"object", "include_targets":[
//@		"../imgproc/split_rgba_image.imgproc.hpp",
//@		"../imgproc/make_rgba_image.imgproc.hpp",
//@		"../imgproc/dft_forward.imgproc.hpp",
//@		"../imgproc/dft_backward.imgproc.hpp"]}]
//@	}

#include "./document.hpp"
#include "filtergraph/image_processor_wrapper.hpp"

//HACK must have a static references to these for linking to work properly
#include "dft/engine.hpp"

#include <algorithm>

namespace
{
	void forceUpdateIfDirty(auto const& item,
	                        Texpainter::Model::Document const& doc,
	                        Texpainter::Model::Document::ForceUpdate force_update)
	{
		if(item.second.source.dirty() || force_update)
		{
			auto node_item = doc.inputNodeItem(item.first);
			assert(node_item != nullptr);
			node_item->second.get().forceUpdate();
		}
	}
}

Texpainter::PixelStore::Image Texpainter::Model::render(Document const& document,
                                                        Document::ForceUpdate force_update,
                                                        uint32_t scale)
{
	std::ranges::for_each(document.images(), [&document, force_update](auto const& item) {
		item.second.processor.get().processor().source(item.second.source.get().pixels());
		forceUpdateIfDirty(item, document, force_update);
	});

	std::ranges::for_each(document.palettes(), [&document, force_update](auto const& item) {
		item.second.processor.get().processor().source(item.second.source.get());
		forceUpdateIfDirty(item, document, force_update);
	});

	PixelStore::Image ret{scale * document.canvasSize().width(),
	                      scale * document.canvasSize().height()};
	if(document.compositor().valid()) [[likely]]
		{
			document.compositor().process(ret.pixels());
		}
	else
	{
		std::ranges::fill(ret.pixels(), PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
	}

	std::ranges::for_each(document.images(),
	                      [&document](auto const& item) { item.second.source.clearStatus(); });

	std::ranges::for_each(document.palettes(),
	                      [&document](auto const& item) { item.second.source.clearStatus(); });

	if(scale == 1) [[likely]]
		{
			return ret;
		}

	PixelStore::Image downsampled{document.canvasSize()};
#if 1
	using InputArray = std::array<FilterGraph::InputPortValue, FilterGraph::NodeArgument::MaxNumInputs>;
	using OutputArray = std::array<FilterGraph::PortValue, FilterGraph::AbstractImageProcessor::MaxNumOutputs>;

	auto rgba = [](auto input) {
		FilterGraph::ImageProcessorWrapper to_grayscale{::SplitRgbaImage::ImageProcessor{}};
		return to_grayscale(FilterGraph::NodeArgument{
		    input.size(),
		   InputArray{
		        input.data()}});
	}(ret.pixels());

	auto dft = [](auto size, auto const& val) {
		FilterGraph::ImageProcessorWrapper dft_forward{::DftForward::ImageProcessor{}};
		return OutputArray{
			std::move(dft_forward(FilterGraph::NodeArgument{size, InputArray{makeInputPortValue(val[0])}})[0]),
			std::move(dft_forward(FilterGraph::NodeArgument{size, InputArray{makeInputPortValue(val[1])}})[0]),
			std::move(dft_forward(FilterGraph::NodeArgument{size, InputArray{makeInputPortValue(val[2])}})[0]),
			std::move(dft_forward(FilterGraph::NodeArgument{size, InputArray{makeInputPortValue(val[3])}})[0])
		};
	}(ret.size(), rgba);

	auto mask = [](uint32_t scale, auto const& spectrum) {
		auto const w_out = spectrum.width()/static_cast<double>(scale);
		auto const h_out = spectrum.height()/static_cast<double>(scale);
		auto const x_min = (spectrum.width() - w_out)/2.0;
		auto const y_min = (spectrum.height() - h_out)/2.0;
		auto const x_max = x_min + w_out;
		auto const y_max = y_min + h_out;
		auto retbuff = std::make_unique<std::complex<double>[]>(area(spectrum.size()));
		Span2d ret{retbuff.get(), spectrum.size()};
		for(uint32_t k = static_cast<uint32_t>(y_min); k < static_cast<uint32_t>(y_max + 0.5); ++k)
		{
			for(uint32_t l = static_cast<uint32_t>(x_min); l < static_cast<uint32_t>(x_max + 0.5); ++l)
			{
				ret(l, k) = spectrum(l, k);
			}
		}
		return retbuff;
	};

	auto masked_dft = [&mask](uint32_t scale, auto size, auto const& output_array) {
		using InputT = std::unique_ptr<std::complex<double>[]>;
		return OutputArray{
			mask(scale, Span2d{Enum::get_if<InputT>(&output_array[0])->get(), size}),
			mask(scale, Span2d{Enum::get_if<InputT>(&output_array[1])->get(), size}),
			mask(scale, Span2d{Enum::get_if<InputT>(&output_array[2])->get(), size}),
			mask(scale, Span2d{Enum::get_if<InputT>(&output_array[3])->get(), size})
		};
	}(scale, ret.size(), dft);

	auto idft = [](auto size, auto const& val){
		FilterGraph::ImageProcessorWrapper dft_backward{::DftBackward::ImageProcessor{}};
		return OutputArray{
			std::move(dft_backward(FilterGraph::NodeArgument{size, InputArray{makeInputPortValue(val[0])}})[0]),
			std::move(dft_backward(FilterGraph::NodeArgument{size, InputArray{makeInputPortValue(val[1])}})[0]),
			std::move(dft_backward(FilterGraph::NodeArgument{size, InputArray{makeInputPortValue(val[2])}})[0]),
			std::move(dft_backward(FilterGraph::NodeArgument{size, InputArray{makeInputPortValue(val[3])}})[0])};
	}(ret.size(), masked_dft);

	auto rgba_filtered = [](auto size, auto const& val) {
		FilterGraph::ImageProcessorWrapper to_rgba{::MakeRgbaImage::ImageProcessor{}};
		using InputT = std::unique_ptr<PixelStore::Pixel[]>;

		auto result = to_rgba(FilterGraph::NodeArgument{size, InputArray{makeInputPortValue(val[0]),
			makeInputPortValue(val[1]),
					   makeInputPortValue(val[2]),
					   makeInputPortValue(val[3])
		}});

		return std::move(*Enum::get_if<InputT>(result.data()));
	}(ret.size(), idft);

	auto rgba_filtered_span = Span2d{rgba_filtered.get(), ret.size()};
	for(uint32_t row = 0; row < document.canvasSize().height(); ++row)
	{
		for(uint32_t col = 0; col < document.canvasSize().width(); ++col)
		{
			downsampled(col, row) = rgba_filtered_span(scale * col, scale * row);
		}
	}

#else

	for(uint32_t row = 0; row < document.canvasSize().height(); ++row)
	{
		for(uint32_t col = 0; col < document.canvasSize().width(); ++col)
		{
			PixelStore::Pixel result{0.0, 0.0, 0.0, 0.0};
			for(uint32_t row_src = 0; row_src < scale; ++row_src)
			{
				for(uint32_t col_src = 0; col_src < scale; ++col_src)
				{
					result += ret(scale * col + col_src, scale * row + row_src);
				}
			}
			downsampled(col, row) = result / static_cast<float>(scale * scale);
		}
	}
#endif
	//	PixelStore::BasicImage<

	return downsampled;
}

void Texpainter::Model::paint(Document& doc, vec2_t location)
{
	auto brush       = doc.currentBrush();
	auto palette_ref = doc.palette(doc.currentPalette());
	if(palette_ref == nullptr) [[unlikely]]
		{
			return;
		}

	auto const& palette = palette_ref->source.get();

	doc.modify(
	    [location,
	     brush_radius = static_cast<double>(brush.radius()),
	     brush_func   = BrushFunction{brush.shape()},
	     color        = palette[doc.currentColor()]](PixelStore::Image& img) noexcept {
		    auto r = 0.5 * std::exp2(brush_radius * 0.5 * std::log2(area(img)));
		    paint(img.pixels(), location, r, brush_func, color);
		    return true;
	    },
	    doc.currentImage());
}

void Texpainter::Model::store(Document const& doc, char const* filename)
{
	nlohmann::json obj{std::pair{"workspace", doc.workspace()},
	                   std::pair{"canvas_size", doc.canvasSize()}};
	auto const str = obj.dump(1, '\t');

	auto const f = fopen(filename, "wb");
	if(f == nullptr) { throw std::string{"Failed to open "} + filename + ": " + strerror(errno); }
	fputs(str.c_str(), f);
	fclose(f);
}