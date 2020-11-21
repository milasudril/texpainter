//@	{
//@	 "targets":[{"name":"document.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"document.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODELNEW_DOCUMENT_HPP
#define TEXPAINTER_MODELNEW_DOCUMENT_HPP

#include "./compositor.hpp"
#include "./item_name.hpp"
#include "./palette.hpp"
#include "./image_source.hpp"

#include "pixel_store/image.hpp"
#include "utils/with_status.hpp"

namespace Texpainter::Model
{
	namespace detail
	{
		template<class T>
		struct ImageProcessor;

		template<>
		struct ImageProcessor<WithStatus<PixelStore::Image>>
		{
			using type = ImageSource;
		};
	}

	template<class T>
	struct CompositorInput
	{
		T source;
		std::reference_wrapper<
		    FilterGraph::ImageProcessorWrapper<typename detail::ImageProcessor<T>::type>>
		    processor;
	};

	class Document
	{
	public:
		explicit Document(Size2d canvas_size): m_canvas_size{canvas_size}, m_dirty{false} {}

		bool dirty() const;

		Size2d canvasSize() const { return m_canvas_size; }

		Document& canvasSize(Size2d size)
		{
			m_canvas_size = size;
			m_dirty       = true;
			return *this;
		}

		Compositor const& compositor() const { return m_compositor; }

		Compositor& compositor() { return m_compositor; }

		auto const& images() const { return m_images; }

		auto const image(ItemName const& item) const
		{
			auto i = m_images.find(item);
			return i != std::end(m_images) ? &i->second : nullptr;
		}

		auto insert(ItemName const& name, PixelStore::Image&& img)
		{
			if(auto node = insertInputNode<ImageSource>(name); node != nullptr) [[likely]]
				{
					return &m_images
					            .insert(std::pair{
					                name, CompositorInput{WithStatus{std::move(img)}, *node}})
					            .first->second;
				}
			return static_cast<CompositorInput<WithStatus<PixelStore::Image>>*>(nullptr);
		}

		template<InplaceMutator<PixelStore::Image> Mutator>
		bool modifyImage(Mutator&& mut, ItemName const& item)
		{
			auto i = m_images.find(item);
			if(i == std::end(m_images)) [[unlikely]]
				{
					return false;
				}

			return i->second.source.modify(std::forward<Mutator>(mut));
		}

		auto const& palettes() const { return m_palettes; }

		auto const palette(ItemName const& item) const
		{
			auto i = m_palettes.find(item);
			return i != std::end(m_palettes) ? &i->second : nullptr;
		}
#if 0
		auto insert(ItemName&& name, Palette&& pal)
		{
			return detail::insert(m_palettes, std::move(name), WithStatus{std::move(pal)});
		}
#endif
		template<InplaceMutator<Palette> Mutator>
		bool modifyPalette(Mutator&& mut, ItemName const& item)
		{
			auto i = m_palettes.find(item);
			if(i == std::end(m_palettes)) [[unlikely]]
				{
					return false;
				}

			return i->second.modify(std::forward<Mutator>(mut));
		}

	private:
		Size2d m_canvas_size;
		Compositor m_compositor;
		std::map<ItemName, CompositorInput<WithStatus<PixelStore::Image>>> m_images;
		std::map<ItemName, WithStatus<Palette>> m_palettes;
		std::map<ItemName, Compositor::NodeItem> m_input_nodes;
		bool m_dirty;

		template<class T>
		FilterGraph::ImageProcessorWrapper<T>* insertInputNode(ItemName const& name)
		{
			auto i = m_input_nodes.find(name);
			if(i == std::end(m_input_nodes)) [[unlikely]]
				{
					return nullptr;
				}

			auto item = m_compositor.insert(T{std::string{toString(name)}});
			m_input_nodes.insert(i, std::pair{name, item});

			return dynamic_cast<FilterGraph::ImageProcessorWrapper<T>*>(
			    &item.second.get().processor());
		}
	};

	inline PixelStore::Image render(Document const& document)
	{
		PixelStore::Image ret{document.canvasSize()};
		document.compositor().process(ret.pixels());
		return ret;
	}
}

#endif
