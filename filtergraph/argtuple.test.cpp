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
static_assert(std::is_same_v<decltype(args.get<0>()), Texpainter::FilterGraph::RealValue const*>);
static_assert(
    std::is_same_v<decltype(args.get<1>()), Texpainter::FilterGraph::ComplexValue const*>);
static_assert(std::is_same_v<decltype(args.get<2>()), Texpainter::PixelStore::Pixel const*>);
static_assert(std::size(args) == std::size(types));

namespace Testcases
{
	void texpainterFilterGraphArgTupleGetValues()
	{
		Texpainter::FilterGraph::RealValue x;
		Texpainter::FilterGraph::ComplexValue y;
		Texpainter::PixelStore::Pixel z;
		MyArgTuple args;
		args.get<0>() = &x;
		args.get<1>() = &y;
		args.get<2>() = &z;

		assert(std::as_const(args).get<0>() == &x);
		assert(std::as_const(args).get<1>() == &y);
		assert(std::as_const(args).get<2>() == &z);
	}
}

int main()
{
	Testcases::texpainterFilterGraphArgTupleGetValues();
	return 0;
}