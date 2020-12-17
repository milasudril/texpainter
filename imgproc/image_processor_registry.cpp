//@	{
//@	"targets":[{"name":"image_processor_registry.o","type":"object",
//@		"include_targets":[
//@		"./add_grayscale_images.imgproc.hpp",
//@		"./add_image_spectra.imgproc.hpp",
//@		"./add_rgba_images.imgproc.hpp",
//@		"./multiply_grayscale_images.imgproc.hpp",
//@		"./multiply_rgba_and_grayscale_images.imgproc.hpp",
//@		"./multiply_image_spectrum_and_grayscale_image.imgproc.hpp",
//@		"./multiply_image_spectra.imgproc.hpp",
//@		"./multiply_rgba_images.imgproc.hpp",
//@		"./mix_rgba_images.imgproc.hpp",
//@		"./mix_grayscale_images.imgproc.hpp",
//@		"./mix_image_spectra.imgproc.hpp",
//@		"./checkerboard.imgproc.hpp",
//@		"./butterworth_mask.imgproc.hpp",
//@		"./gaussian_mask.imgproc.hpp",
//@		"./grayscale_noise.imgproc.hpp",
//@		"./rgba_noise.imgproc.hpp",
//@		"./real_constant.imgproc.hpp",
//@		"./split_rgba_image.imgproc.hpp",
//@		"./rgba_to_gray_and_alpha.imgproc.hpp",
//@		"./make_rgba_image.imgproc.hpp",
//@		"./apply_colormap.imgproc.hpp",
//@		"./dft_forward.imgproc.hpp",
//@		"./dft_backward.imgproc.hpp",
//@		"./make_topographic_map.imgproc.hpp",
//@		"./project_normal_vector.imgproc.hpp",
//@		"./normalize_grayscale_image.imgproc.hpp",
//@		"./soft_clamp_grayscale_image.imgproc.hpp",
//@		"./translate_grayscale_image.imgproc.hpp",
//@		"./translate_rgba_image.imgproc.hpp",
//@		"./scale_grayscale_image.imgproc.hpp",
//@		"./scale_rgba_image.imgproc.hpp"
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
	    get_info<::SoftClampGrayscaleImage::ImageProcessor>(),
	    get_info<::TranslateGrayscaleImage::ImageProcessor>(),
	    get_info<::TranslateRgbaImage::ImageProcessor>(),
	    get_info<::ScaleGrayscaleImage::ImageProcessor>(),
	    get_info<::ScaleRgbaImage::ImageProcessor>()};

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