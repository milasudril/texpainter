//@	{"targets":[{"name":"compositing_options_editor.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_COMPOSITINGEDITOROPTIONS_HPP
#define TEXPAINTER_APP_COMPOSITINGEDITOROPTIONS_HPP

#include "./filter_graph_editor.hpp"

#include "model/compositing_options.hpp"

namespace Texpainter
{
	class CompositingOptionsEditor
	{
	public:
		explicit CompositingOptionsEditor(Ui::Container& container,
		                                  Model::CompositingOptions const& comp_opts,
		                                  std::map<FilterGraph::NodeId, vec2_t> const& node_locs)
		    : m_root{container, Ui::Box::Orientation::Horizontal}
		    , m_filter_graph{m_root,
		                     Ui::Box::Orientation::Vertical,
		                     "Filter graph",
		                     comp_opts.filterGraph(),
		                     node_locs}
		{
		}

		Model::CompositingOptions compositingOptions() const
		{
			return Model::CompositingOptions{m_filter_graph.inputField().filterGraph(),
			                                 Model::BlendFunction{Model::BlendMode::SourceOver},
			                                 1.0f};
		}

		decltype(auto) nodeLocations() const { return m_filter_graph.inputField().nodeLocations(); }

		template<auto id, class EventHandler>
		CompositingOptionsEditor& eventHandler(EventHandler& eh)
		{
			m_filter_graph.inputField().template eventHandler<id>(eh);
			return *this;
		}

	private:
		Ui::Box m_root;
		Ui::LabeledInput<FilterGraphEditor> m_filter_graph;
	};
}

#endif
