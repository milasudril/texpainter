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
#include "ui/separator.hpp"
#include "pixel_store/hsi_rgb.hpp"

#include "libenum/enum.hpp"

namespace Texpainter::App
{
	namespace detail
	{
		constexpr auto NumPortTypes = Enum::distance(begin(Enum::Empty<FilterGraph::PortType>{}),
		                                             end(Enum::Empty<FilterGraph::PortType>{}));

		constexpr std::array<PixelStore::RgbaValue, NumPortTypes> genPortColors()
		{
			std::array<PixelStore::RgbaValue, NumPortTypes> ret{};

			auto dh = 1.0f / NumPortTypes;

			for(std::remove_const_t<decltype(NumPortTypes)> k = 0; k < NumPortTypes; ++k)
			{
				ret[k] = toRgb(PixelStore::Hsi{dh * static_cast<float>(k), 1.0f, 0.5f, 1.0f});
			}

			return ret;
		}

		constexpr auto portColors = genPortColors();

		constexpr PixelStore::RgbaValue portColor(FilterGraph::PortType type)
		{
			return portColors[Enum::distance(begin(Enum::Empty<FilterGraph::PortType>{}), type)];
		}

		template<class PortIndex>
		constexpr unsigned short insertPosition() = delete;

		template<>
		constexpr unsigned short insertPosition<FilterGraph::InputPortIndex>()
		{
			return 0;
		}

		template<>
		constexpr unsigned short insertPosition<FilterGraph::OutputPortIndex>()
		{
			return Ui::Box::PositionBack;
		}

		template<class PortIndex, class EventHandler>
		class Connector
		{
		public:
			using type = PortIndex;

			explicit Connector(Ui::Container& owner, PortIndex port, FilterGraph::PortInfo info)
			    : m_port{port}
			    , m_root{owner, Ui::Box::Orientation::Horizontal}
			    , m_connector{m_root.insertMode(
			                      Ui::Box::InsertMode{0, insertPosition<PortIndex>()}),
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

			template<int>
			void onCompleted(Ui::FilledShape&)
			{
				r_eh->onCompleted(*this);
			}

			template<int>
			void handleException(char const* msg, Ui::FilledShape&)
			{
				r_eh->handleException(msg, *this);
			}

		private:
			EventHandler* r_eh;
			PortIndex m_port;
			Ui::Box m_root;
			Ui::FilledShape m_connector;
			Ui::Label m_label;
		};

		template<class EventHandler>
		class ParamInput
		{
		public:
			explicit ParamInput(Ui::Container& owner, FilterGraph::ParamName name)
			    : m_name{name}
			    , m_slider{owner, Ui::Box::Orientation::Vertical, name.c_str(), false}
			{
			}

			void eventHandler(EventHandler& eh)
			{
				r_eh = &eh;
				m_slider.inputField().eventHandler<0>(*this);
			}

			template<int>
			void onChanged(Ui::Slider&)
			{
				r_eh->onChanged(*this);
			}

			auto name() const { return m_name; }

			auto value() const
			{
				return FilterGraph::ParamValue{
				    static_cast<float>(m_slider.inputField().value().value())};
			}

			void value(FilterGraph::ParamValue val)
			{
				m_slider.inputField().value(Ui::SliderValue{val.value()});
			}


		private:
			EventHandler* r_eh;
			FilterGraph::ParamName m_name;
			Ui::LabeledInput<Ui::Slider> m_slider;
		};

		template<class Connector>
		class ConnectorFactory
		{
		public:
			explicit ConnectorFactory(Ui::Container& owner): r_owner{owner}, m_k{0} {}

			auto operator()(auto portinfo)
			{
				if(portinfo.hidden) { return std::optional<Connector>{}; }
				return std::optional{Connector{r_owner, typename Connector::type{m_k++}, portinfo}};
			}

		private:
			std::reference_wrapper<Ui::Container> r_owner;
			uint32_t m_k;
		};
	}

	template<class EventHandler>
	class NodeEditor
	{
	public:
		using InputConnector  = detail::Connector<FilterGraph::InputPortIndex, NodeEditor>;
		using OutputConnector = detail::Connector<FilterGraph::OutputPortIndex, NodeEditor>;
		using ParamInput      = detail::ParamInput<NodeEditor>;

		NodeEditor(Ui::Container& owner, std::reference_wrapper<FilterGraph::Node> node)
		    : r_node{node}
		    , m_root{owner, Ui::Box::Orientation::Vertical}
		    , m_name{m_root,
		             std::to_string(node.get().nodeId().value())
		                 .append(" ")
		                 .append(node.get().name())
		                 .c_str()}
		    , m_content{m_root, Ui::Box::Orientation::Horizontal}
		    , m_input_col{m_content.insertMode(Ui::Box::InsertMode{0, 0}),
		                  Ui::Box::Orientation::Vertical}
		    , m_input_sep{m_content.insertMode(Ui::Box::InsertMode{2, 0})}
		    , m_params_col{m_content.insertMode(
		                       Ui::Box::InsertMode{0, Ui::Box::Expand | Ui::Box::Fill}),
		                   Ui::Box::Orientation::Vertical}
		    , m_output_sep{m_content.insertMode(Ui::Box::InsertMode{2, 0})}
		    , m_output_col{m_content.insertMode(Ui::Box::InsertMode{0, 0}),
		                   Ui::Box::Orientation::Vertical}
		    , m_num_completed_connectors{0}
		{
			m_name.oneline(true).alignment(0.5);

			std::ranges::transform(r_node.get().inputPorts(),
			                       std::back_inserter(m_inputs),
			                       detail::ConnectorFactory<InputConnector>{m_input_col});

			std::ranges::transform(r_node.get().paramNames(),
			                       std::back_inserter(m_params),
			                       [&params = m_params_col](auto name) {
				                       return ParamInput{params, name};
			                       });

			std::ranges::transform(r_node.get().outputPorts(),
			                       std::back_inserter(m_outputs),
			                       detail::ConnectorFactory<OutputConnector>{m_output_col});

			std::ranges::for_each(m_params, [&node = r_node.get()](auto& param) {
				param.value(node.get(param.name()));
			});
		}

		auto const& inputs() const { return m_inputs; }

		auto const& outputs() const { return m_outputs; }

		auto& node() const { return r_node.get(); }

		NodeEditor& eventHandler(EventHandler& eh)
		{
			std::ranges::for_each(m_inputs, [this](auto& item) {
				if(item) [[likely]]
					{
						item->eventHandler(*this);
					}
			});
			std::ranges::for_each(m_outputs, [this](auto& item) {
				if(item) [[likely]]
					{
						item->eventHandler(*this);
					}
			});
			std::ranges::for_each(m_params, [this](auto& item) { item.eventHandler(*this); });
			r_eh = &eh;
			return *this;
		}

		template<class Connector>
		void onClicked(Connector const& src)
		{
			r_eh->onClicked(*this, src.port());
		}

		void onChanged(ParamInput const& input)
		{
			r_eh->onChanged(*this, input.name(), input.value());
		}

		template<class Connector>
		void handleException(char const* msg, Connector& src)
		{
			r_eh->handleException(msg, *this, src.port());
		}

		template<class Connector>
		void onCompleted(Connector const& src)
		{
			++m_num_completed_connectors;
			if(m_num_completed_connectors == m_inputs.size() + m_outputs.size())
			{ r_eh->onCompleted(*this, src.port()); }
		}

		void update() { m_name.content(r_node.get().name()); }


	private:
		std::reference_wrapper<FilterGraph::Node> r_node;
		EventHandler* r_eh;

		Ui::Box m_root;
		Ui::Label m_name;
		Ui::Box m_content;
		Ui::Box m_input_col;
		Ui::Separator m_input_sep;
		Ui::Box m_params_col;
		Ui::Separator m_output_sep;
		Ui::Box m_output_col;

		std::vector<std::optional<InputConnector>> m_inputs;
		std::vector<ParamInput> m_params;
		std::vector<std::optional<OutputConnector>> m_outputs;
		size_t m_num_completed_connectors;
	};
}

#endif
