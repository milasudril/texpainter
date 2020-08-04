//@	{
//@	 "targets":[{"name":"processor_node.test", "type":"application", "autorun":1}]
//@	}

#include "./processor_node.hpp"

#include <cassert>
#include <map>

namespace
{
	class InputProcessor
	{
		static constexpr char const* s_output_port_names[] = {"Output 1", "Output 2"};

	public:
		InputProcessor(size_t& req_count): r_req_count{req_count}
		{
			set("color_a", Texpainter::FilterGraph::ProcParamValue{1.0});
			set("color_b", Texpainter::FilterGraph::ProcParamValue{1.0});
		}

		static constexpr std::span<char const* const> paramNames()
		{
			return std::span<char const* const>{};
		}

		std::span<Texpainter::FilterGraph::ProcParamValue const> paramValues() const
		{
			return std::span<Texpainter::FilterGraph::ProcParamValue const>{};
		}

		static constexpr std::span<char const* const> inputPorts()
		{
			return std::span<char const* const>{};
		}

		static constexpr std::span<char const* const> outputPorts()
		{
			return s_output_port_names;
		}


		std::vector<Texpainter::FilterGraph::ProcResultType> operator()(
		    std::span<Texpainter::FilterGraph::ProcArgumentType const>) const
		{
			auto const color_a = m_params.at("color_a");
			Texpainter::PixelStore::Image ret_a{1, 1};
			ret_a.pixels()(0, 0) = Texpainter::PixelStore::Pixel{0.0f, color_a, 0.0f, 0.0f};

			auto const color_b = m_params.at("color_b");
			Texpainter::PixelStore::Image ret_b{1, 1};
			ret_b.pixels()(0, 0) = Texpainter::PixelStore::Pixel{0.0f, 0.0f, color_b, 0.0f};

			++r_req_count;
			return std::vector<Texpainter::FilterGraph::ProcResultType>{std::move(ret_a),
			                                                            std::move(ret_b)};
		}

		Texpainter::FilterGraph::ProcParamValue get(std::string_view) const
		{
			return Texpainter::FilterGraph::ProcParamValue{0.0};
		}

		InputProcessor& set(std::string_view param, Texpainter::FilterGraph::ProcParamValue val)
		{
			m_params[std::string{param}] = static_cast<float>(val.value());
			return *this;
		}

		size_t inputCount() const { return 0; }

		static constexpr char const* name() { return "Input proc"; }


	private:
		std::map<std::string, float, std::less<>> m_params;
		size_t& r_req_count;
	};

	class TestProcessor
	{
		static constexpr char const* s_output_port_names[] = {"Output 1", "Output 2"};
		static constexpr char const* s_input_port_names[]  = {"Input 1", "Input 2"};
		static constexpr char const* s_param_names[]  = {"Foo", "Bar"};

	public:
		size_t inputCount() const { return 2; }

		static constexpr std::span<char const* const> paramNames()
		{
			return s_param_names;
		}

		std::span<Texpainter::FilterGraph::ProcParamValue const> paramValues() const
		{
			return std::span<Texpainter::FilterGraph::ProcParamValue const>{};
		}

		static constexpr std::span<char const* const> inputPorts()
		{
			return s_input_port_names;
		}

		static constexpr std::span<char const* const> outputPorts()
		{
			return s_output_port_names;
		}


		std::vector<Texpainter::FilterGraph::ProcResultType> operator()(
		    std::span<Texpainter::FilterGraph::ProcArgumentType const> args) const
		{
			std::vector<Texpainter::FilterGraph::ProcResultType> ret{};
			assert(std::size(args) == 2);
			for(int k = 0; k < 2; ++k)
			{
				if(auto val = std::get_if<Texpainter::Span2d<Texpainter::PixelStore::Pixel const>>(
				       &args[k]);
				   val != nullptr)
				{
					Texpainter::PixelStore::Image img{1, 1};
					img.pixels()(0, 0) = Texpainter::PixelStore::Pixel{1.0f, 0.0f, 0.0f, 1.0f};
					if(val->area() != 0) { img.pixels()(0, 0) += (*val)(0, 0); }
					ret.push_back(std::move(img));
				}
			}
			return ret;
		}

		Texpainter::FilterGraph::ProcParamValue get(std::string_view param_name) const
		{
			auto i = m_params.find(param_name);
			if(i != std::end(m_params))
			{ return Texpainter::FilterGraph::ProcParamValue{i->second}; }
			return Texpainter::FilterGraph::ProcParamValue{0.0};
		}

		TestProcessor& set(std::string_view param_name,
		                   Texpainter::FilterGraph::ProcParamValue value)
		{
			m_params[std::string{param_name}] = value.value();
			return *this;
		}

		static constexpr char const* name() { return "Test proc"; }

	private:
		std::map<std::string, double, std::less<>> m_params;
	};
}

namespace Testcases
{
	void filtergraphProcessorNodeDummy()
	{
		Texpainter::FilterGraph::ProcessorNode node;

		auto res = node();
		assert(res.size() == 0);

		assert(&node.set("Foo", Texpainter::FilterGraph::ProcParamValue{0.5}) == &node);
		assert(node.get("Foo") == Texpainter::FilterGraph::ProcParamValue{0.0});
		assert(node.inputPorts().size() == 0);
	}

	void filtergraphProcessorNodeGetOutput()
	{
		Texpainter::FilterGraph::ProcessorNode node{TestProcessor{}};

		assert(node.inputPorts().size() == 2);
		node.set("Foo", Texpainter::FilterGraph::ProcParamValue{0.5});
		assert(node.get("Foo") == Texpainter::FilterGraph::ProcParamValue{0.5});

		size_t req_count = 0;

		Texpainter::FilterGraph::ProcessorNode src{InputProcessor{req_count}};
		node.connect(
		        Texpainter::FilterGraph::InputPort{0}, src, Texpainter::FilterGraph::OutputPort{0})
		    .connect(
		        Texpainter::FilterGraph::InputPort{1}, src, Texpainter::FilterGraph::OutputPort{1});

		auto res = node();
		assert(std::size(res) == 2);
		assert(req_count == 1);

		auto res_0 = get_if<Texpainter::PixelStore::Image>(&res[0]);
		auto res_1 = get_if<Texpainter::PixelStore::Image>(&res[1]);
		assert(res_0 != nullptr);
		assert(res_1 != nullptr);

		assert(res_0->pixels()(0, 0).red() == 1.0f);
		assert(res_0->pixels()(0, 0).green() == 1.0f);
		assert(res_0->pixels()(0, 0).blue() == 0.0f);
		assert(res_0->pixels()(0, 0).alpha() == 1.0f);

		assert(res_1->pixels()(0, 0).red() == 1.0f);
		assert(res_1->pixels()(0, 0).green() == 0.0f);
		assert(res_1->pixels()(0, 0).blue() == 1.0f);
		assert(res_1->pixels()(0, 0).alpha() == 1.0f);
	}

	void filtergraphProcessorNodeDestroySourceGetOutput()
	{
		Texpainter::FilterGraph::ProcessorNode node{TestProcessor{}};

		assert(node.inputPorts().size() == 2);
		node.set("Foo", Texpainter::FilterGraph::ProcParamValue{0.5});
		assert(node.get("Foo") == Texpainter::FilterGraph::ProcParamValue{0.5});

		size_t req_count = 0;

		{
			Texpainter::FilterGraph::ProcessorNode src{InputProcessor{req_count}};
			node.connect(Texpainter::FilterGraph::InputPort{0},
			             src,
			             Texpainter::FilterGraph::OutputPort{0})
			    .connect(Texpainter::FilterGraph::InputPort{1},
			             src,
			             Texpainter::FilterGraph::OutputPort{1});
		}

		auto res = node();
		assert(std::size(res) == 2);
		assert(req_count == 0);

		auto res_0 = get_if<Texpainter::PixelStore::Image>(&res[0]);
		auto res_1 = get_if<Texpainter::PixelStore::Image>(&res[1]);
		assert(res_0 != nullptr);
		assert(res_1 != nullptr);

		assert(res_0->pixels()(0, 0).red() == 1.0f);
		assert(res_0->pixels()(0, 0).green() == 0.0f);
		assert(res_0->pixels()(0, 0).blue() == 0.0f);
		assert(res_0->pixels()(0, 0).alpha() == 1.0f);

		assert(res_1->pixels()(0, 0).red() == 1.0f);
		assert(res_1->pixels()(0, 0).green() == 0.0f);
		assert(res_1->pixels()(0, 0).blue() == 0.0f);
		assert(res_1->pixels()(0, 0).alpha() == 1.0f);
	}

	void filtergraphProcessorNodeDestroyCycle()
	{
		Texpainter::FilterGraph::ProcessorNode node_1{TestProcessor{}};
		Texpainter::FilterGraph::ProcessorNode node_2{TestProcessor{}};
		Texpainter::FilterGraph::ProcessorNode node_3{TestProcessor{}};

		node_1.connect(
		    Texpainter::FilterGraph::InputPort{0}, node_2, Texpainter::FilterGraph::OutputPort{0});
		node_2.connect(
		    Texpainter::FilterGraph::InputPort{0}, node_3, Texpainter::FilterGraph::OutputPort{0});
		node_3.connect(
		    Texpainter::FilterGraph::InputPort{0}, node_1, Texpainter::FilterGraph::OutputPort{0});
	}

	void filtergraphProcessorNodeModifyInput()
	{
		Texpainter::FilterGraph::ProcessorNode node{TestProcessor{}};

		assert(node.inputPorts().size() == 2);
		node.set("Foo", Texpainter::FilterGraph::ProcParamValue{0.5});
		assert(node.get("Foo") == Texpainter::FilterGraph::ProcParamValue{0.5});

		size_t req_count = 0;

		Texpainter::FilterGraph::ProcessorNode src{InputProcessor{req_count}};
		node.connect(
		        Texpainter::FilterGraph::InputPort{0}, src, Texpainter::FilterGraph::OutputPort{0})
		    .connect(
		        Texpainter::FilterGraph::InputPort{1}, src, Texpainter::FilterGraph::OutputPort{1});

		assert(node.dirty());
		auto& res = node();

		assert(!node.dirty());
		auto res_0 = get_if<Texpainter::PixelStore::Image>(&res[0]);
		auto res_1 = get_if<Texpainter::PixelStore::Image>(&res[1]);
		assert(res_0->pixels()(0, 0).green() == 1.0f);
		assert(res_1->pixels()(0, 0).blue() == 1.0f);


		src.set("color_a", Texpainter::FilterGraph::ProcParamValue{0.75});
		src.set("color_b", Texpainter::FilterGraph::ProcParamValue{0.25});
		assert(node.dirty());
		node();
		res_0 = get_if<Texpainter::PixelStore::Image>(&res[0]);
		res_1 = get_if<Texpainter::PixelStore::Image>(&res[1]);
		assert(!node.dirty());
		assert(res_0->pixels()(0, 0).green() == 0.75f);
		assert(res_1->pixels()(0, 0).blue() == 0.25f);
	}
};

int main()
{
	Testcases::filtergraphProcessorNodeDummy();
	Testcases::filtergraphProcessorNodeGetOutput();
	Testcases::filtergraphProcessorNodeDestroySourceGetOutput();
	Testcases::filtergraphProcessorNodeDestroyCycle();
	Testcases::filtergraphProcessorNodeModifyInput();
	return 0;
}