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

		template<>
		struct ImageProcessor<WithStatus<Palette>>
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

	template<class T>
	class CompositorInputManager
	{
	public:
		auto const& get(std::type_identity<T>) const { return m_inputs; }

		auto const get(std::type_identity<T>, ItemName const& item) const
		{
			auto i = m_inputs.find(item);
			return i != std::end(m_inputs) ? &i->second : nullptr;
		}

		auto insert(ItemName const& name,
		            T&& img,
		            Compositor& compositor,
		            std::map<ItemName, Compositor::NodeItem>& nodes)
		{
			auto i = nodes.find(name);
			if(i == std::end(nodes)) [[unlikely]]
				{
					return static_cast<CompositorInput<WithStatus<T>>*>(nullptr);
				}

			using ImgProc = typename detail::ImageProcessor<WithStatus<T>>::type;
			auto item     = compositor.insert(ImgProc{std::string{toString(name)}});
			nodes.insert(i, std::pair{name, item});
			auto& node = dynamic_cast<FilterGraph::ImageProcessorWrapper<ImgProc>&>(
			    item.second.get().processor());
			return &m_inputs
			            .insert(std::pair{name, CompositorInput{WithStatus{std::move(img)}, node}})
			            .first->second;
		}

		bool erase(std::type_identity<T>,
		           ItemName const& item,
		           Compositor& compositor,
		           std::map<ItemName, Compositor::NodeItem>& nodes)
		{
			auto i = nodes.find(item);
			if(i == std::end(nodes)) [[unlikely]]
				{
					return false;
				}

			compositor.erase(i->second.first);
			nodes.erase(i);
			m_inputs.erase(item);
		}

		template<InplaceMutator<T> Mutator>
		bool modify(Mutator&& mut, ItemName const& item)
		{
			auto i = m_inputs.find(item);
			if(i == std::end(m_inputs)) [[unlikely]]
				{
					return false;
				}

			return i->second.source.modify(std::forward<Mutator>(mut));
		}

	private:
		std::map<ItemName, CompositorInput<WithStatus<T>>> m_inputs;
	};

	class Document: private Size2d,
	                private Compositor,
	                private CompositorInputManager<PixelStore::Image>,
	                private CompositorInputManager<Palette>
	{
		using CompositorInputManager<PixelStore::Image>::insert;
		using CompositorInputManager<Palette>::insert;

	public:
		using CompositorInputManager<PixelStore::Image>::get;
		using CompositorInputManager<PixelStore::Image>::modify;
		using CompositorInputManager<PixelStore::Image>::erase;
		using CompositorInputManager<Palette>::get;
		using CompositorInputManager<Palette>::modify;
		using CompositorInputManager<Palette>::erase;

		explicit Document(Size2d canvas_size): Size2d{canvas_size}, m_dirty{false} {}

		bool dirty() const;

		Size2d canvasSize() const { return static_cast<Size2d>(*this); }

		Document& canvasSize(Size2d size)
		{
			static_cast<Size2d&>(*this) = size;
			m_dirty                     = true;
			return *this;
		}

		Compositor const& compositor() const { return static_cast<Compositor const&>(*this); }

		Compositor& compositor() { return static_cast<Compositor&>(*this); }


		auto const& images() const { return get(std::type_identity<PixelStore::Image>{}); }

		auto image(ItemName const& name) const
		{
			return get(std::type_identity<PixelStore::Image>{}, name);
		}

		auto insert(ItemName const& name, PixelStore::Image&& img)
		{
			return insert(name, std::forward<PixelStore::Image>(img), *this, m_input_nodes);
		}


		auto const& palettes() const { return get(std::type_identity<Palette>{}); }

		auto palette(ItemName const& name) const
		{
			return get(std::type_identity<Palette>{}, name);
		}

		auto insert(ItemName const& name, Palette&& pal)
		{
			return insert(name, std::forward<Palette>(pal), *this, m_input_nodes);
		}

	private:
		std::map<ItemName, Compositor::NodeItem> m_input_nodes;
		bool m_dirty;
	};

	inline PixelStore::Image render(Document const& document)
	{
		PixelStore::Image ret{document.canvasSize()};
		document.compositor().process(ret.pixels());
		return ret;
	}
}

#endif
