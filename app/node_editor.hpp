//@	{
//@	"targets":[{"name":"node_editor.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_NODEEDITOR_HPP
#define TEXPAINTER_APP_NODEEDITOR_HPP

#include "filtergraph/node.hpp"
#include "ui/label.hpp"
#include "ui/box.hpp"
#include "ui/slider.hpp"
#include "ui/filled_shape.hpp"
#include "ui/labeled_input.hpp"

namespace Texpainter
{
	namespace detail
	{
		constexpr PixelStore::Pixel portColor(FilterGraph::PixelType type)
		{
			switch(type)
			{
				case FilterGraph::PixelType::RGBA: return PixelStore::Pixel{0.5f, 0.5f, 0.0f, 1.0f};

				case FilterGraph::PixelType::GrayscaleReal:
					return PixelStore::Pixel{0.33f, 0.33f, 0.33f, 1.0f};

				case FilterGraph::PixelType::GrayscaleComplex:
					return PixelStore::Pixel{0.25f, 0.25f, 1.0f, 1.0f};
			}
			__builtin_unreachable();
		}

		constexpr auto box_radius = 1.0 - 1.0 / std::numbers::phi;
		constexpr auto box_offset = vec2_t{0.5, 0.0};

		class InputConnector
		{
		public:
			explicit InputConnector(Ui::Container& owner,
			                        FilterGraph::InputPort port,
			                        FilterGraph::PortInfo info)
			    : m_port{port}
			    , m_root{owner, Ui::Box::Orientation::Horizontal}
			    , m_connector{m_root, portColor(info.type)}
			    , m_label{m_root, info.name}
			{
			}

			auto location() const { return m_connector.location(); }

		private:
			FilterGraph::InputPort m_port;
			Ui::Box m_root;
			Ui::FilledShape m_connector;
			Ui::Label m_label;
		};

		class OutputConnector
		{
		public:
			explicit OutputConnector(Ui::Container& owner,
			                         FilterGraph::OutputPort port,
			                         FilterGraph::PortInfo info)
			    : m_port{port}
			    , m_root{owner, Ui::Box::Orientation::Horizontal}
			    , m_label{m_root, info.name}
			    , m_connector{m_root, portColor(info.type)}
			{
			}

			auto location() const { return m_connector.location(); }

		private:
			FilterGraph::OutputPort m_port;
			Ui::Box m_root;
			Ui::Label m_label;
			Ui::FilledShape m_connector;
		};
	}


	class NodeEditor
	{
		enum class ControlId : int
		{
			Input,
			Output
		};

	public:
		NodeEditor(Ui::Container& owner, std::reference_wrapper<FilterGraph::Node const> node)
		    : r_node{node}
		    , m_root{owner, Ui::Box::Orientation::Vertical}
		    , m_name{m_root, node.get().name()}
		    , m_content{m_root, Ui::Box::Orientation::Horizontal}
		    , m_inputs{m_content.insertMode(Ui::Box::InsertMode{0, 0}),
		               Ui::Box::Orientation::Vertical}
		    , m_params{m_content.insertMode(
		                   Ui::Box::InsertMode{0, Ui::Box::Expand | Ui::Box::Fill}),
		               Ui::Box::Orientation::Vertical}
		    , m_outputs{m_content.insertMode(Ui::Box::InsertMode{0, 0}),
		                Ui::Box::Orientation::Vertical}
		{
			m_name.oneline(true).alignment(0.5);
			std::ranges::transform(
			    r_node.get().inputPorts(),
			    std::back_inserter(m_input_labels),
			    [&owner = m_inputs, k = 0u](auto portinfo) mutable {
				    auto ret = detail::InputConnector{owner, FilterGraph::InputPort{k}, portinfo};
				    ++k;
				    return ret;
			    });

			std::ranges::transform(
			    r_node.get().paramNames(),
			    std::back_inserter(m_params_input),
			    [&params = m_params](auto name) {
				    return Ui::LabeledInput<Ui::Slider>{
				        params, Ui::Box::Orientation::Vertical, name.c_str(), false};
			    });

			std::ranges::transform(
			    r_node.get().outputPorts(),
			    std::back_inserter(m_output_labels),
			    [&owner = m_outputs, k = 0u](auto portinfo) mutable {
				    auto ret = detail::OutputConnector{owner, FilterGraph::OutputPort{k}, portinfo};
				    ++k;
				    return ret;
			    });

			//		std::ranges::for_each(m_input_labels, [this](auto& item) {
			//			item.inputField().template eventHandler<ControlId::Input>(*this);
			//		});

			//		std::ranges::for_each(m_output_labels, [this](auto& item) {
			//			item.inputField().template eventHandler<ControlId::Output>(*this);
			//		});
		}

		auto const& inputs() const { return m_input_labels; }

		auto const& outputs() const { return m_output_labels; }

		auto node() const { return r_node; }

		template<ControlId>
		void onClicked(Ui::FilledShape&);

	private:
		std::reference_wrapper<FilterGraph::Node const> r_node;

		Ui::Box m_root;
		Ui::Label m_name;
		Ui::Box m_content;
		Ui::Box m_inputs;
		Ui::Box m_params;
		Ui::Box m_outputs;

		std::vector<detail::InputConnector> m_input_labels;
		std::vector<Ui::LabeledInput<Ui::Slider>> m_params_input;
		std::vector<detail::OutputConnector> m_output_labels;
	};

	template<>
	inline void NodeEditor::onClicked<NodeEditor::ControlId::Output>(Ui::FilledShape& src)
	{
		puts("Output");
	}

	template<>
	inline void NodeEditor::onClicked<NodeEditor::ControlId::Input>(Ui::FilledShape& src)
	{
		puts("Input");
	}
}

#endif
