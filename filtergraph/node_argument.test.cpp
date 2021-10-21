//@	{
//@	 "targets":[{"name":"node_argument.test", "type":"application", "autorun":1}]
//@	}

#include "./node_argument.hpp"

#include "./argtuple.hpp"

#include <cassert>
#include <cstring>

constexpr auto types = std::array<Texpainter::FilterGraph::PortType, 3>{
    Texpainter::FilterGraph::PortType::GrayscaleRealPixels,
    Texpainter::FilterGraph::PortType::GrayscaleComplexPixels,
    Texpainter::FilterGraph::PortType::RgbaPixels,
};

using MyInArgTuple = Texpainter::FilterGraph::InArgTuple<types>;

namespace Testcases
{
	void texpainterFilterGraphNodeArgumentGetPointers()
	{
		Texpainter::FilterGraph::RealValue const a{};
		Texpainter::FilterGraph::ComplexValue const b{};
		Texpainter::FilterGraph::RgbaValue const c{};

		Texpainter::Size2d size{23, 56};
		Texpainter::FilterGraph::NodeArgument test{
		    size, 2.0, Texpainter::DefaultRng::SeedValue{3455}, {&a, &b, &c}};

		assert(test.size() == size);
		assert(test.resolution() == 2.0);
		assert((test.rngSeed() == Texpainter::DefaultRng::SeedValue{3455}));

		auto const& inputs = test.inputs();
		assert(*Enum::get_if<Texpainter::FilterGraph::RealValue const*>(&inputs[0]) == &a);
		assert(*Enum::get_if<Texpainter::FilterGraph::ComplexValue const*>(&inputs[1]) == &b);
		assert(*Enum::get_if<Texpainter::FilterGraph::RgbaValue const*>(&inputs[2]) == &c);
	}
}

int main()
{
	Testcases::texpainterFilterGraphNodeArgumentGetPointers();
	return 0;
}