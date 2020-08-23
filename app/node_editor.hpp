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

		template<class PortType>
		constexpr unsigned short insertPosition() = delete;

		template<>
		constexpr unsigned short insertPosition<FilterGraph::InputPort>()
		{
			return 0;
		}

		template<>
		constexpr unsigned short insertPosition<FilterGraph::OutputPort>()
		{
			return Ui::Box::PositionBack;
		}

		template<class PortType, class EventHandler>
		class Connector
		{
		public:
			using type = PortType;

			explicit Connector(Ui::Container& owner, type port, FilterGraph::PortInfo info)
			    : m_port{port}
			    , m_root{owner, Ui::Box::Orientation::Horizontal}
			    , m_connector{m_root.insertMode(Ui::Box::InsertMode{0, insertPosition<type>()}),
			                  portColor(info.type)}
			    , m_label{m_root, info.name}
			{
			}

			auto location() const { return m_connector.location(); }

			auto port() const { return m_port; }

			void eventHandler(EventHandler& eh)
			{
				r_eh = &eh;
				m_connector.eventHandler<0>(*this);
			}

			template<int>
			void onClicked(Ui::FilledShape&)
			{
				r_eh->onClicked(*this);
			}

		private:
			EventHandler* r_eh;
			type m_port;
			Ui::Box m_root;
			Ui::FilledShape m_connector;
			Ui::Label m_label;
		};

		template<class PortType, class EventHandler>
		class PortCreator
		{
		public:
			explicit PortCreator(Ui::Container& owner): r_owner{owner}, m_k{0} {}

			auto operator()(auto portinfo)
			{
				return Connector<PortType, EventHandler>{r_owner, PortType{m_k++}, portinfo};
			}

		private:
			std::reference_wrapper<Ui::Container> r_owner;
			uint32_t m_k;
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
			    detail::PortCreator<FilterGraph::InputPort, NodeEditor>{m_inputs});

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
			    detail::PortCreator<FilterGraph::OutputPort, NodeEditor>{m_outputs});

			std::ranges::for_each(m_input_labels, [this](auto& item) { item.eventHandler(*this); });

			std::ranges::for_each(m_output_labels,
			                      [this](auto& item) { item.eventHandler(*this); });
		}

		auto const& inputs() const { return m_input_labels; }

		auto const& outputs() const { return m_output_labels; }

		auto node() const { return r_node; }

		template<class Connector>
		void onClicked(Connector const& src)
		{
			if constexpr(std::is_same_v<typename Connector::type, FilterGraph::InputPort>)
			{ printf("Input  %u\n", src.port().value()); }
			else
			{
				printf("Output %u\n", src.port().value());
			}
		}


	private:
		std::reference_wrapper<FilterGraph::Node const> r_node;

		Ui::Box m_root;
		Ui::Label m_name;
		Ui::Box m_content;
		Ui::Box m_inputs;
		Ui::Box m_params;
		Ui::Box m_outputs;

		std::vector<detail::Connector<FilterGraph::InputPort, NodeEditor>> m_input_labels;
		std::vector<Ui::LabeledInput<Ui::Slider>> m_params_input;
		std::vector<detail::Connector<FilterGraph::OutputPort, NodeEditor>> m_output_labels;
	};
}

#endif
