//@	{
//@	"targets":[{"name":"image_processor_registry.o","type":"object",
//@		"include_targets":[
//@		"../imgproc_new/add_grayscale_images.imgproc.hpp",
//@		"../imgproc_new/add_image_spectra.imgproc.hpp",
//@		"../imgproc_new/add_rgba_images.imgproc.hpp",
//@		"../imgproc_new/multiply_grayscale_images.imgproc.hpp"
//@	]}]
//@	}

#include "./image_processor_registry.hpp"

#include "./rgba_combine/rgba_combine.hpp"
#include "./rgba_split/rgba_split.hpp"
#include "./gaussian_mask_2d/gaussian_mask_2d.hpp"
#include "./dft_forward/dft_forward.hpp"
#include "./dft_backward/dft_backward.hpp"
#include "./complex_real_multiply/complex_real_multiply.hpp"
#include "./butterworth_mask_2d/butterworth_mask_2d.hpp"
#include "./butterworth_mask_1d/butterworth_mask_1d.hpp"
#include "./normalize_grayscale/normalize_grayscale.hpp"
#include "./grayscale_noise/grayscale_noise.hpp"
#include "./mix_grayscale/mix_grayscale.hpp"
#include "./mix_complex/mix_complex.hpp"
#include "./real_constant/real_constant.hpp"
#include "./grayscale_range/grayscale_range.hpp"
#include "./map_grayscale_range/map_grayscale_range.hpp"
#include "./random_color/random_color.hpp"
#include "./checkerboard/checkerboard.hpp"
#include "./rgba_to_grayalpha/rgba_to_grayalpha.hpp"
#include "./soft_clamp/soft_clamp.hpp"
#include "./topographic_map/topographic_map.hpp"
#include "./project_normal/project_normal.hpp"

#include "filtergraph/image_processor_wrapper.hpp"
#include "utils/fixed_flatmap.hpp"

namespace
{
	using ImageProcessorFactory =
	    std::unique_ptr<Texpainter::FilterGraph::AbstractImageProcessor> (*)();

	struct ImageProcessorInfo
	{
		char const* name;
		Texpainter::FilterGraph::ImageProcessorId id;
		ImageProcessorFactory factory;
	};

	template<Texpainter::FilterGraph::ImageProcessor Proc>
	constexpr ImageProcessorInfo get_info()
	{
		return ImageProcessorInfo{
		    Proc::name(), Proc::id(), Texpainter::FilterGraph::createImageProcessor<Proc>};
	}

	constexpr ImageProcessorInfo s_processors[] = {
		get_info<::AddImageSpectra::ImageProcessor>(),
		get_info<::AddGrayscaleImages::ImageProcessor>(),
		get_info<::AddRgbaImages::ImageProcessor>(),
		get_info<::MultiplyGrayscaleImages::ImageProcessor>(),
	    get_info<::RgbaCombine::ImageProcessor>(),
	    get_info<::RgbaSplit::ImageProcessor>(),
	    get_info<::GaussianMask2d::ImageProcessor>(),
	    get_info<::DftForward::ImageProcessor>(),
	    get_info<::DftBackward::ImageProcessor>(),
	    get_info<::ComplexRealMultiply::ImageProcessor>(),
	    get_info<::ButterworthMask2d::ImageProcessor>(),
	    get_info<::ButterworthMask1d::ImageProcessor>(),
	    get_info<::NormalizeGrayscale::ImageProcessor>(),
	    get_info<::GrayscaleNoise::ImageProcessor>(),
	    get_info<::MixGrayscale::ImageProcessor>(),
	    get_info<::MixComplex::ImageProcessor>(),
	    get_info<::RealConstant::ImageProcessor>(),
	    get_info<::GrayscaleRange::ImageProcessor>(),
	    get_info<::MapGrayscaleRange::ImageProcessor>(),
	    get_info<::RandomColor::ImageProcessor>(),
	    get_info<::Checkerboard::ImageProcessor>(),
	    get_info<::RgbaToGrayAlpha::ImageProcessor>(),
	    get_info<::SoftClamp::ImageProcessor>(),
	    get_info<::TopographicMap::ImageProcessor>(),
	    get_info<::ProjectNormal::ImageProcessor>()};

	template<class Keys, class Value, class Compare>
	using FixedFlatmap = Texpainter::FixedFlatmap<Keys, Value, Compare>;

	struct CompareId
	{
		constexpr bool operator()(Texpainter::FilterGraph::ImageProcessorId const& a,
		                          Texpainter::FilterGraph::ImageProcessorId const& b) const
		{
			return std::ranges::lexicographical_compare(a.data(), b.data());
		}
	};

	constexpr auto get_img_proc_ids()
	{
		std::array<Texpainter::FilterGraph::ImageProcessorId, std::size(s_processors)> ret;
		for(size_t k = 0; k < std::size(ret); ++k)
		{
			ret[k] = s_processors[k].id;
		}
		return ret;
	}

	struct ImageProcessorIds
	{
		static constexpr auto items = get_img_proc_ids();
	};

	using IdToProcMap = FixedFlatmap<ImageProcessorIds, ImageProcessorFactory, CompareId>;


	struct CompareName
	{
		constexpr bool operator()(char const* a, char const* b) const { return strcmp(a, b) < 0; }
	};

	constexpr auto get_img_proc_names()
	{
		std::array<char const*, std::size(s_processors)> ret;
		for(size_t k = 0; k < std::size(ret); ++k)
		{
			ret[k] = s_processors[k].name;
		}
		return ret;
	}

	struct ImageProcessorNames
	{
		static constexpr auto items = get_img_proc_names();
	};

	using NameToProcMap = FixedFlatmap<ImageProcessorNames, ImageProcessorFactory, CompareName>;

	constexpr auto make_id_to_processor()
	{
		IdToProcMap ret;
		for(size_t k = 0; k < std::size(s_processors); ++k)
		{
			*ret.find(s_processors[k].id) = s_processors[k].factory;
		}
		return ret;
	}

	constexpr auto make_name_to_processor()
	{
		NameToProcMap ret;
		for(size_t k = 0; k < std::size(s_processors); ++k)
		{
			*ret.find(s_processors[k].name) = s_processors[k].factory;
		}
		return ret;
	}

	constexpr auto id_to_processor   = make_id_to_processor();
	constexpr auto name_to_processor = make_name_to_processor();
}


std::unique_ptr<Texpainter::FilterGraph::AbstractImageProcessor> Texpainter::
    ImageProcessorRegistry::createImageProcessor(
        Texpainter::FilterGraph::ImageProcessorId const& id)
{
	auto i = id_to_processor.find(id);
	return i != nullptr ? (*i)() : nullptr;
}

std::unique_ptr<Texpainter::FilterGraph::AbstractImageProcessor> Texpainter::
    ImageProcessorRegistry::createImageProcessor(char const* name)
{
	auto i = name_to_processor.find(name);
	return i != nullptr ? (*i)() : nullptr;
}

std::span<Texpainter::FilterGraph::ImageProcessorId const> Texpainter::ImageProcessorRegistry::
    processorIds()
{
	return id_to_processor.keys();
}

std::span<char const* const> Texpainter::ImageProcessorRegistry::processorNames()
{
	return name_to_processor.keys();
}