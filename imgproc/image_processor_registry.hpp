//@	{
//@	 "targets":[{"name":"image_processor_registry.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORREGISTRY_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORREGISTRY_HPP

#include "./rgba_combine/rgba_combine.hpp"
#include "./rgba_split/rgba_split.hpp"

#include "filtergraph/image_processor_wrapper.hpp"

namespace Texpainter
{
	namespace detail
	{
		using ImageProcessorFactory = std::unique_ptr<FilterGraph::AbstractImageProcessor> (*)();
		struct ImageProcessorInfo
		{
			char const* name;
			FilterGraph::ImageProcessorId id;
			ImageProcessorFactory factory;
		};

		template<FilterGraph::ImageProcessor Proc>
		constexpr ImageProcessorInfo get_info()
		{
			return ImageProcessorInfo{
			    Proc::name(), Proc::id(), FilterGraph::createProcInstance<Proc>};
		}

		constexpr ImageProcessorInfo processors[] = {get_info<::RgbaCombine::ImageProcessor>(),
		                                             get_info<::RgbaSplit::ImageProcessor>()};

		constexpr auto img_proc_id_to_factory()
		{
			std::array<std::pair<FilterGraph::ImageProcessorId, ImageProcessorFactory>,
			           std::size(processors)>
			    ret;
			for(size_t k = 0; k < std::size(ret); ++k)
			{
				ret[k] = {processors[k].id, processors[k].factory};
			}

			std::ranges::sort(ret, [](auto const& a, auto const& b) {
				return std::ranges::lexicographical_compare(a.first.data(), b.first.data());
			});
			return ret;
		}

		constexpr auto img_proc_name_to_factory()
		{
			std::array<std::pair<char const*, ImageProcessorFactory>, std::size(processors)> ret;
			for(size_t k = 0; k < std::size(ret); ++k)
			{
				ret[k] = {processors[k].name, processors[k].factory};
			}

			std::ranges::sort(
			    ret, [](auto const& a, auto const& b) { return strcmp(a.first, b.first) < 0; });
			return ret;
		}
	}

	class ImageProcessorRegistry
	{
	public:
	private:
		static constexpr auto s_name_to_factory = detail::img_proc_name_to_factory();
		static constexpr auto s_id_to_factory   = detail::img_proc_id_to_factory();
	};
}

#endif