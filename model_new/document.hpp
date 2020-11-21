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
		template<class Value>
		auto insert(std::map<ItemName, Value>& map, ItemName&& name, Value&& val)
		{
			auto ip = map.insert(std::make_pair(std::move(name), std::move(val)));
			if(ip.second) [[unlikely]]
				{
					return static_cast<Value*>(nullptr);
				}

			return &ip.first->second;
		}
	}


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
			auto i = m_input_nodes.find(name);
			if(i == std::end(m_input_nodes)) [[unlikely]]
			{ return static_cast<WithStatus<PixelStore::Image>*>(nullptr); }

			auto item = m_compositor.insert(ImageSource{std::string{toString(name)}});
			m_input_nodes.insert(i, std::pair{name, item});

			return &m_images.insert(std::pair{name, WithStatus{std::move(img)}}).first->second;
		}

		template<InplaceMutator<PixelStore::Image> Mutator>
		bool modifyImage(Mutator&& mut, ItemName const& item)
		{
			auto i = m_images.find(item);
			if(i == std::end(m_images)) [[unlikely]]
				{
					return false;
				}

			return i->second.modify(std::forward<Mutator>(mut));
		}

		auto const& palettes() const { return m_palettes; }

		auto const palette(ItemName const& item) const
		{
			auto i = m_palettes.find(item);
			return i != std::end(m_palettes) ? &i->second : nullptr;
		}

		auto insert(ItemName&& name, Palette&& pal)
		{
			return detail::insert(m_palettes, std::move(name), WithStatus{std::move(pal)});
		}

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
		std::map<ItemName, WithStatus<PixelStore::Image>> m_images;
		std::map<ItemName, WithStatus<Palette>> m_palettes;
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
