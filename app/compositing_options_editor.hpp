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
		enum class ControlId : int
		{
			FilterGraph,
			BlendFunction,
			LayerOpacity
		};

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
		          m_footer.insertMode(Ui::Box::InsertMode{8, Ui::Box::Fill | Ui::Box::Expand}),
		          Ui::Box::Orientation::Horizontal,
		          "Layer opacity: ",
		          false}
		{
			forEachEnumItem<Model::BlendMode>([&blend_func = m_blend_func.inputField()](auto tag) {
				blend_func.append(Model::BlendModeTraits<tag.value>::displayName());
			});
			m_blend_func.inputField().selected(static_cast<int>(comp_opts.blendFunction().id()));
			m_layer_opacity.inputField().value(Ui::SliderValue{comp_opts.opacity()});
		}

		Model::CompositingOptions compositingOptions() const
		{
			return Model::CompositingOptions{
			    std::ref(m_filter_graph.inputField().filterGraph()),
			    Model::BlendFunction{
			        static_cast<Model::BlendMode>(m_blend_func.inputField().selected())},
			    static_cast<float>(m_layer_opacity.inputField().value().value())};
		}

		Model::CompositingOptions compositingOptionsWithGraph() const
		{
			return Model::CompositingOptions{FilterGraph::Graph{m_filter_graph.inputField().filterGraph()},
			    Model::BlendFunction{
			        static_cast<Model::BlendMode>(m_blend_func.inputField().selected())},
			    static_cast<float>(m_layer_opacity.inputField().value().value())};
		}

		decltype(auto) nodeLocations() const { return m_filter_graph.inputField().nodeLocations(); }

		template<auto id, class EventHandler>
		CompositingOptionsEditor& eventHandler(EventHandler& eh)
		{
			r_eh       = &eh;
			r_callback = [](void* eh, CompositingOptionsEditor& self) {
				reinterpret_cast<EventHandler*>(eh)->template onChanged<id>(self);
			};
			m_filter_graph.inputField().template eventHandler<ControlId::FilterGraph>(*this);
			m_blend_func.inputField().template eventHandler<ControlId::BlendFunction>(*this);
			m_layer_opacity.inputField().template eventHandler<ControlId::LayerOpacity>(*this);
			return *this;
		}

		template<ControlId>
		void graphUpdated(FilterGraphEditor const&)
		{
			r_callback(r_eh, *this);
		}

		template<ControlId>
		void onChanged(Ui::Combobox const&)
		{
			r_callback(r_eh, *this);
		}

		template<ControlId>
		void onChanged(Ui::Slider const&)
		{
			r_callback(r_eh, *this);
		}

		template<ControlId, class... T>
		void handleException(char const* msg, T&...)
		{
			fprintf(stderr, "An error occured while updating the view: %s\n", msg);
			std::terminate();
		}

	private:
		void* r_eh;
		void (*r_callback)(void* eh, CompositingOptionsEditor& self);
		Ui::Box m_root;
		Ui::LabeledInput<FilterGraphEditor> m_filter_graph;
		Ui::Box m_footer;
		Ui::LabeledInput<Ui::Combobox> m_blend_func;
		Ui::LabeledInput<Ui::Slider> m_layer_opacity;
	};
}

#endif
