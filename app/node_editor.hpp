//@	{
//@	"targets":[{"name":"node_editor.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_NODEEDITOR_HPP
#define TEXPAINTER_NODEEDITOR_HPP

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
	}


	class NodeEditor
	{
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
			constexpr auto box_radius = 1.0 - 1.0 / std::numbers::phi;
			constexpr auto box_offset = vec2_t{0.5, 0.0};

			m_name.alignment(0.5);
			std::ranges::transform(r_node.get().inputPorts(),
			                       std::back_inserter(m_input_labels),
			                       [&inputs = m_inputs, box_radius, box_offset](auto portinfo) {
				                       auto ret = Ui::ReversedLabeledInput<Ui::FilledShape>{
				                           inputs,
				                           Ui::Box::Orientation::Horizontal,
				                           portinfo.name,
				                           detail::portColor(portinfo.type),
				                           box_radius,
				                           -box_offset};
				                       ret.label().alignment(0.0);
				                       return ret;
			                       });

			std::ranges::transform(r_node.get().paramNames(),
			                       std::back_inserter(m_params_input),
			                       [&params = m_params](auto name) {
				                       return Ui::LabeledInput<Ui::Slider>{
				                           params, Ui::Box::Orientation::Vertical, name, false};
			                       });

			std::ranges::transform(r_node.get().outputPorts(),
			                       std::back_inserter(m_output_labels),
			                       [&outputs = m_outputs, box_radius, box_offset](auto portinfo) {
				                       auto ret = Ui::LabeledInput<Ui::FilledShape>{
				                           outputs,
				                           Ui::Box::Orientation::Horizontal,
				                           portinfo.name,
				                           detail::portColor(portinfo.type),
				                           box_radius,
				                           box_offset};
				                       ret.label().alignment(1.0);
				                       return ret;
			                       });
		}

		auto const& inputs() const { return m_input_labels; }

		auto const& outputs() const { return m_output_labels; }

		auto node() const { return r_node; }


	private:
		std::reference_wrapper<FilterGraph::Node const> r_node;

		Ui::Box m_root;
		Ui::Label m_name;
		Ui::Box m_content;
		Ui::Box m_inputs;
		Ui::Box m_params;
		Ui::Box m_outputs;

		std::vector<Ui::ReversedLabeledInput<Ui::FilledShape>> m_input_labels;
		std::vector<Ui::LabeledInput<Ui::Slider>> m_params_input;
		std::vector<Ui::LabeledInput<Ui::FilledShape>> m_output_labels;
	};
}

#endif
