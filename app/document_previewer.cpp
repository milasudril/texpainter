//@	{
//@	 "targets":[{"name":"document_previewer.o", "type":"object"}]
//@	}

#include "./document_previewer.hpp"

#include "ui/context.hpp"

void Texpainter::App::DocumentPreviewer::refreshNodeSelector()
{
	m_node_selector.clear();
	m_index_to_node.clear();

	auto const& nodes    = m_doc.get().compositor().nodesWithId();
	auto index_to_select = static_cast<size_t>(-1);
	std::ranges::for_each(
	    nodes,
	    [&node_selector = m_node_selector,
	     &node_selected = m_doc.get().compositor().outputNode(),
	     &index_to_select,
	     &index_to_node = m_index_to_node,
	     index          = static_cast<size_t>(0)](auto const& item) mutable {
		    node_selector.append(
		        std::to_string(item.first.value()).append(" ").append(item.second.name()).c_str());
		    if(&item.second == &node_selected) { index_to_select = index; }
		    index_to_node.push_back(item.second);
		    ++index;
	    });
	m_node_selector.selected(static_cast<int>(index_to_select));
}

namespace
{
	class ViewUpdater
	{
	public:
		explicit ViewUpdater(Texpainter::Size2d canvas_size,
		                     Texpainter::Ui::WidgetMultiplexer& views,
		                     Texpainter::Ui::ImageView& img_view,
		                     Texpainter::App::TerrainView& terrain_view)
		    : m_canvas_size{canvas_size}
		    , m_views{views}
		    , m_img_view{img_view}
		    , m_terrain_view{terrain_view}
		{
		}

		template<class T>
		void operator()(T const&) const
		{
		}

		void operator()(std::unique_ptr<Texpainter::PixelStore::Pixel[]> const& data)
		{
			m_views.get().showWidget("imgview");
			m_img_view.get().image(Texpainter::Span2d{data.get(), m_canvas_size});
		}

		void operator()(std::unique_ptr<Texpainter::Model::TopographyInfo[]> const& data)
		{
			m_views.get().showWidget("terrainview");
			m_terrain_view.get().topography(Texpainter::Span2d{data.get(), m_canvas_size});
		}

	private:
		Texpainter::Size2d m_canvas_size;
		std::reference_wrapper<Texpainter::Ui::WidgetMultiplexer> m_views;
		std::reference_wrapper<Texpainter::Ui::ImageView> m_img_view;
		std::reference_wrapper<Texpainter::App::TerrainView> m_terrain_view;
	};
}

Texpainter::App::DocumentPreviewer& Texpainter::App::DocumentPreviewer::refreshImageView()
{
	auto result = render(m_doc.get(), Model::Document::ForceUpdate{false});
	visit(ViewUpdater{result.size(), m_views, m_img_view, m_terrain_view}, result.data());
	return *this;
}
