//@	{
//@	"targets":[{"name":"image_processor_registry.o","type":"object",
//@		"include_targets":[
//@		"../imgproc_new/add_grayscale_images.imgproc.hpp",
//@		"../imgproc_new/add_image_spectra.imgproc.hpp",
//@		"../imgproc_new/add_rgba_images.imgproc.hpp",
//@		"../imgproc_new/multiply_grayscale_images.imgproc.hpp",
//@		"../imgproc_new/multiply_rgba_and_grayscale_images.imgproc.hpp",
//@		"../imgproc_new/multiply_image_spectrum_and_grayscale_image.imgproc.hpp",
//@		"../imgproc_new/multiply_image_spectra.imgproc.hpp",
//@		"../imgproc_new/multiply_rgba_images.imgproc.hpp",
//@		"../imgproc_new/mix_rgba_images.imgproc.hpp",
//@		"../imgproc_new/mix_grayscale_images.imgproc.hpp",
//@		"../imgproc_new/mix_image_spectra.imgproc.hpp",
//@		"../imgproc_new/checkerboard.imgproc.hpp",
//@		"../imgproc_new/butterworth_mask.imgproc.hpp",
//@		"../imgproc_new/gaussian_mask.imgproc.hpp",
//@		"../imgproc_new/grayscale_noise.imgproc.hpp",
//@		"../imgproc_new/rgba_noise.imgproc.hpp",
//@		"../imgproc_new/real_constant.imgproc.hpp",
//@		"../imgproc_new/split_rgba_image.imgproc.hpp",
//@		"../imgproc_new/rgba_to_gray_and_alpha.imgproc.hpp",
//@		"../imgproc_new/make_rgba_image.imgproc.hpp",
//@		"../imgproc_new/apply_colormap.imgproc.hpp",
//@		"../imgproc_new/dft_forward.imgproc.hpp",
//@		"../imgproc_new/dft_backward.imgproc.hpp",
//@		"../imgproc_new/make_topographic_map.imgproc.hpp",
//@		"../imgproc_new/project_normal_vector.imgproc.hpp",
//@		"../imgproc_new/normalize_grayscale_image.imgproc.hpp",
//@		"../imgproc_new/soft_clamp_grayscale_image.imgproc.hpp"
//@	]}]
//@	}

#include "./image_processor_registry.hpp"

#include "filtergraph/image_processor_wrapper.hpp"
#include "utils/fixed_flatmap.hpp"

//HACK must have a static references to these for linking to work properly
#include "utils/default_rng.hpp"
#include "dft/engine.hpp"

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
	    get_info<::MultiplyRgbaAndGrayscaleImages::ImageProcessor>(),
	    get_info<::MultiplyImageSpectrumAndGrayscaleImage::ImageProcessor>(),
	    get_info<::MultiplyImageSpectra::ImageProcessor>(),
	    get_info<::MultiplyRgbaImages::ImageProcessor>(),
	    get_info<::MixRgbaImages::ImageProcessor>(),
	    get_info<::MixGrayscaleImages::ImageProcessor>(),
	    get_info<::MixImageSpectra::ImageProcessor>(),
	    get_info<::Checkerboard::ImageProcessor>(),
	    get_info<::ButterworthMask::ImageProcessor>(),
	    get_info<::GaussianMask::ImageProcessor>(),
	    get_info<::GrayscaleNoise::ImageProcessor>(),
	    get_info<::RealConstant::ImageProcessor>(),
	    get_info<::RgbaNoise::ImageProcessor>(),
	    get_info<::SplitRgbaImage::ImageProcessor>(),
	    get_info<::RgbaToGrayAndAlpha::ImageProcessor>(),
	    get_info<::MakeRgbaImage::ImageProcessor>(),
	    get_info<::ApplyColormap::ImageProcessor>(),
	    get_info<::DftForward::ImageProcessor>(),
	    get_info<::DftBackward::ImageProcessor>(),
	    get_info<::MakeTopographicMap::ImageProcessor>(),
	    get_info<::ProjectNormalVector::ImageProcessor>(),
	    get_info<::NormalizeGrayscaleImage::ImageProcessor>(),
	    get_info<::SoftClampGrayscaleImage::ImageProcessor>()};

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