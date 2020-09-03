//@	{
//@	"targets":[{"name":"image_processor_registry.o","type":"object"}]
//@	}

#include "./image_processor_registry.hpp"

#include "./rgba_combine/rgba_combine.hpp"
#include "./rgba_split/rgba_split.hpp"
#include "./gaussian_blur/gaussian_blur.hpp"
#include "./dft_forward/dft_forward.hpp"
#include "./dft_backward/dft_backward.hpp"

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

	constexpr ImageProcessorInfo s_processors[] = {get_info<::RgbaCombine::ImageProcessor>(),
	                                               get_info<::RgbaSplit::ImageProcessor>(),
	                                               get_info<::GaussianBlur::ImageProcessor>(),
	                                               get_info<::DftForward::ImageProcessor>(),
	                                               get_info<::DftBackward::ImageProcessor>()};

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