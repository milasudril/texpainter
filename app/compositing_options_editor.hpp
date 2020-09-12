//@	{"targets":[{"name":"compositing_options_editor.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_COMPOSITINGEDITOROPTIONS_HPP
#define TEXPAINTER_APP_COMPOSITINGEDITOROPTIONS_HPP

#include "./filter_graph_editor.hpp"

#include "model/compositing_options.hpp"
#include "ui/combobox.hpp"
#include "ui/slider.hpp"
#include "ui/labeled_input.hpp"

namespace Texpainter
{
	class CompositingOptionsEditor
	{
	public:
		explicit CompositingOptionsEditor(Ui::Container& container,
		                                  Model::CompositingOptions const& comp_opts,
		                                  std::map<FilterGraph::NodeId, vec2_t> const& node_locs)
		    : m_root{container, Ui::Box::Orientation::Vertical}
		    , m_filter_graph{m_root.insertMode(
		                         Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand}),
		                     Ui::Box::Orientation::Vertical,
		                     "Filter graph",
		                     comp_opts.filterGraph(),
		                     node_locs}

		    , m_footer{m_root.insertMode(Ui::Box::InsertMode{0, 0}),
		               Ui::Box::Orientation::Horizontal}
		    , m_blend_func{m_footer, Ui::Box::Orientation::Horizontal, "Blend mode: "}
		    , m_layer_opacity{
		          m_footer.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand}),
		          Ui::Box::Orientation::Horizontal,
		          "Layer opacity: ",
		          false}
		{
			forEachEnumItem<Model::BlendMode>([&blend_func = m_blend_func.inputField()](auto tag) {
				blend_func.append(Model::BlendModeTraits<tag.value>::displayName());
			});
			m_blend_func.inputField().selected(static_cast<int>(comp_opts.blendFunction().id()));
			m_layer_opacity.inputField().value(Ui::SliderValue{comp_opts.strength()});
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
		Ui::Box m_footer;
		Ui::LabeledInput<Ui::Combobox> m_blend_func;
		Ui::LabeledInput<Ui::Slider> m_layer_opacity;
	};
}

#endif
