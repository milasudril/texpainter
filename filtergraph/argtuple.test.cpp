//@	{
//@	 "targets":[{"name":"argtuple.test", "type":"application", "autorun":1}]
//@	}

#include "./argtuple.hpp"

#include <cassert>

constexpr auto types = std::array<Texpainter::FilterGraph::PixelType, 3>{
    Texpainter::FilterGraph::PixelType::GrayscaleReal,
    Texpainter::FilterGraph::PixelType::GrayscaleComplex,
    Texpainter::FilterGraph::PixelType::RGBA,
};

using MyArgTuple = Texpainter::FilterGraph::ArgTuple<types>;

constexpr auto args = MyArgTuple{};

static_assert(sizeof(MyArgTuple) == 3 * sizeof(void*));
static_assert(std::is_same_v<decltype(Texpainter::FilterGraph::get<0>(args)),
                             Texpainter::FilterGraph::RealValue const*>);
static_assert(std::is_same_v<decltype(Texpainter::FilterGraph::get<1>(args)),
                             Texpainter::FilterGraph::ComplexValue const*>);
static_assert(std::is_same_v<decltype(Texpainter::FilterGraph::get<2>(args)),
                             Texpainter::PixelStore::Pixel const*>);

namespace Testcases
{
	void texpainterFilterGraphArgTupleGetValues()
	{
		Texpainter::FilterGraph::RealValue x;
		Texpainter::FilterGraph::ComplexValue y;
		Texpainter::PixelStore::Pixel z;
		MyArgTuple args;
		Texpainter::FilterGraph::get<0>(args) = &x;
		Texpainter::FilterGraph::get<1>(args) = &y;
		Texpainter::FilterGraph::get<2>(args) = &z;

		assert(Texpainter::FilterGraph::get<0>(args) == &x);
		assert(Texpainter::FilterGraph::get<1>(args) == &y);
		assert(Texpainter::FilterGraph::get<2>(args) == &z);
	}
}

int main()
{
	Testcases::texpainterFilterGraphArgTupleGetValues();
	return 0;
}