//@	{
//@	"targets":[{"name":"node_editor.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_NODEEDITOR_HPP
#define TEXPAINTER_NODEEDITOR_HPP

#include "filtergraph/processor_node.hpp"
#include "ui/label.hpp"
#include "ui/box.hpp"
#include "ui/slider.hpp"

namespace Texpainter
{
	class NodeEditor
	{
	public:
		NodeEditor(Ui::Container& owner,
		           std::reference_wrapper<FilterGraph::ProcessorNode const> node)
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
			m_name.alignment(0.5);
			std::ranges::transform(r_node.get().inputPorts(),
			                       std::back_inserter(m_input_labels),
			                       [&inputs = m_inputs](auto name) {
				                       auto ret = Ui::Label{inputs, name};
				                       ret.alignment(0.0);
				                       return ret;
			                       });

			std::ranges::transform(
			    r_node.get().paramNames(),
			    std::back_inserter(m_params_input),
			    [&params = m_params](auto name) {
				    return Ui::LabeledInput<Ui::Slider>{
				        params, Ui::Box::Orientation::Vertical, name, false};
			    });

			std::ranges::transform(r_node.get().outputPorts(),
			                       std::back_inserter(m_output_labels),
			                       [&outputs = m_outputs](auto name) {
				                       auto ret = Ui::Label{outputs, name};
				                       ret.alignment(1.0);
				                       return ret;
			                       });
		}

	private:
		std::reference_wrapper<FilterGraph::ProcessorNode const> r_node;

		Ui::Box m_root;
		Ui::Label m_name;
		Ui::Box m_content;
		Ui::Box m_inputs;
		Ui::Box m_params;
		Ui::Box m_outputs;

		std::vector<Ui::Label> m_input_labels;
		std::vector<Ui::LabeledInput<Ui::Slider>> m_params_input;
		std::vector<Ui::Label> m_output_labels;
	};
}

#endif
