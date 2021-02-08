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
//@		"./checkerboard_pattern.imgproc.hpp",
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
//@		"./scale_rgba_image.imgproc.hpp",
//@		"./rotate_grayscale_image.imgproc.hpp",
//@		"./rotate_rgba_image.imgproc.hpp",
//@		"./rgba_source_over.imgproc.hpp",
//@		"./normalize_rgba_image.imgproc.hpp",
//@		"./make_radial_gradient.imgproc.hpp",
//@		"./exponential_decay.imgproc.hpp",
//@		"./butterworth_decay.imgproc.hpp",
//@		"./gingham_pattern.imgproc.hpp",
//@		"./soft_clamp_rgba_image.imgproc.hpp",
//@		"./grayscale_wave_distortion.imgproc.hpp",
//@		"./grayscale_power_distortion.imgproc.hpp",
//@		"./image_spectrum_to_amplitude_and_phase.imgproc.hpp",
//@		"./random_point_cloud.imgproc.hpp",
//@		"./place_rgba_image_at_points.imgproc.hpp",
//@		"./select_rgba_image.imgproc.hpp"
//@	]}]
//@	}

#include "./image_processor_registry.hpp"

#include "filtergraph/image_processor_wrapper.hpp"
#include "utils/fixed_flatmap.hpp"

//HACK must have a static references to these for linking to work properly
#include "utils/default_rng.hpp"
#include "utils/discrete_pdf.hpp"
#include "dft/engine.hpp"

namespace
{
	using ImageProcessorFactory =
	    std::unique_ptr<Texpainter::FilterGraph::AbstractImageProcessor> (*)();

	struct ImageProcessorDetailedInfo
	{
		Texpainter::ImageProcessorRegistry::ImageProcessorInfo info;
		ImageProcessorFactory factory;
	};

	template<Texpainter::FilterGraph::ImageProcessor Proc>
	constexpr ImageProcessorDetailedInfo get_info()
	{
		return ImageProcessorDetailedInfo{Proc::id(),
		                                  Proc::name(),
		                                  Proc::category(),
		                                  Texpainter::FilterGraph::createImageProcessor<Proc>};
	}

	constexpr ImageProcessorDetailedInfo s_processors[] = {
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
	    get_info<::CheckerboardPattern::ImageProcessor>(),
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
	    get_info<::ScaleRgbaImage::ImageProcessor>(),
	    get_info<::RotateGrayscaleImage::ImageProcessor>(),
	    get_info<::RotateRgbaImage::ImageProcessor>(),
	    get_info<::RgbaSourceOver::ImageProcessor>(),
	    get_info<::NormalizeRgbaImage::ImageProcessor>(),
	    get_info<::MakeRadialGradient::ImageProcessor>(),
	    get_info<::ExponentialDecay::ImageProcessor>(),
	    get_info<::ButterworthDecay::ImageProcessor>(),
	    get_info<::GinghamPattern::ImageProcessor>(),
	    get_info<::SoftClampRgbaImage::ImageProcessor>(),
	    get_info<::GrayscaleWaveDistortion::ImageProcessor>(),
	    get_info<::GrayscalePowerDistortion::ImageProcessor>(),
	    get_info<::ImageSpectrumToAmplitudeAndPhase::ImageProcessor>(),
	    get_info<::RandomPointCloud::ImageProcessor>(),
	    get_info<::PlaceRgbaImageAtPoints::ImageProcessor>(),
	    get_info<::SelectRgbaImage::ImageProcessor>()};

	template<class Keys, class Value, class Compare>
	using FixedFlatmap = Texpainter::FixedFlatmap<Keys, Value, Compare>;

	struct CompareFunc
	{
		constexpr bool operator()(Texpainter::FilterGraph::ImageProcessorId const& a,
		                          Texpainter::FilterGraph::ImageProcessorId const& b) const
		{
			return std::ranges::lexicographical_compare(a.data(), b.data());
		}

		constexpr bool operator()(char const* a, char const* b) const { return strcmp(a, b) < 0; }
	};

	constexpr auto get_img_proc_ids()
	{
		std::array<Texpainter::FilterGraph::ImageProcessorId, std::size(s_processors)> ret;
		for(size_t k = 0; k < std::size(ret); ++k)
		{
			ret[k] = s_processors[k].info.id;
		}
		return ret;
	}

	struct ImageProcessorIds
	{
		static constexpr auto items = get_img_proc_ids();
	};

	using IdToProcMap = FixedFlatmap<ImageProcessorIds, ImageProcessorFactory, CompareFunc>;

	constexpr auto get_img_proc_names()
	{
		std::array<char const*, std::size(s_processors)> ret;
		for(size_t k = 0; k < std::size(ret); ++k)
		{
			ret[k] = s_processors[k].info.name;
		}
		return ret;
	}

	struct ImageProcessorNames
	{
		static constexpr auto items = get_img_proc_names();
	};

	using NameToProcMap = FixedFlatmap<ImageProcessorNames, ImageProcessorFactory, CompareFunc>;

	constexpr auto make_id_to_processor()
	{
		IdToProcMap ret;
		for(size_t k = 0; k < std::size(s_processors); ++k)
		{
			*ret.find(s_processors[k].info.id) = s_processors[k].factory;
		}
		return ret;
	}

	constexpr auto make_name_to_processor()
	{
		NameToProcMap ret;
		for(size_t k = 0; k < std::size(s_processors); ++k)
		{
			*ret.find(s_processors[k].info.name) = s_processors[k].factory;
		}
		return ret;
	}

	constexpr auto make_imgproc_list()
	{
		std::array<Texpainter::ImageProcessorRegistry::ImageProcessorInfo, std::size(s_processors)>
		    ret;
		std::ranges::transform(
		    s_processors, std::begin(ret), [](auto const& val) { return val.info; });
		return ret;
	}

	constexpr auto make_imgprocs_by_id()
	{
		auto ret = make_imgproc_list();
		std::ranges::sort(ret,
		                  [](auto const& a, auto const& b) { return CompareFunc{}(a.id, b.id); });
		return ret;
	}

	constexpr auto make_imgprocs_by_name()
	{
		auto ret = make_imgproc_list();
		std::ranges::sort(ret, [](auto const& a, auto const& b) {
			auto const res = strcmp(a.name, b.name);
			if(res < 0) { return true; }
			if(res == 0) { return strcmp(a.category, b.category) < 0; }
			return false;
		});
		return ret;
	}

	constexpr auto make_imgprocs_by_category()
	{
		auto ret = make_imgproc_list();
		std::ranges::sort(ret, [](auto const& a, auto const& b) {
			auto const res = strcmp(a.category, b.category);
			if(res < 0) { return true; }
			if(res == 0) { return strcmp(a.name, b.name) < 0; }
			return false;
		});
		return ret;
	}

	constexpr auto id_to_processor   = make_id_to_processor();
	constexpr auto name_to_processor = make_name_to_processor();

	constexpr auto imgprocs_by_id       = make_imgprocs_by_id();
	constexpr auto imgprocs_by_name     = make_imgprocs_by_name();
	constexpr auto imgprocs_by_category = make_imgprocs_by_category();
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

std::span<Texpainter::ImageProcessorRegistry::ImageProcessorInfo const> Texpainter::
    ImageProcessorRegistry::imageProcessorsById()
{
	return imgprocs_by_id;
}

std::span<Texpainter::ImageProcessorRegistry::ImageProcessorInfo const> Texpainter::
    ImageProcessorRegistry::imageProcessorsByName()
{
	return imgprocs_by_name;
}

std::span<Texpainter::ImageProcessorRegistry::ImageProcessorInfo const> Texpainter::
    ImageProcessorRegistry::imageProcessorsByCategory()
{
	return imgprocs_by_category;
}
