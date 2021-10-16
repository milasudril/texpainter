//@	{
//@	 "targets":[{"name":"argtuple.test", "type":"application", "autorun":1}]
//@	}

#include "./argtuple.hpp"

#include <cassert>

constexpr auto types = std::array<Texpainter::FilterGraph::PortType, 3>{
    Texpainter::FilterGraph::PortType::GrayscaleRealPixels,
    Texpainter::FilterGraph::PortType::GrayscaleComplexPixels,
    Texpainter::FilterGraph::PortType::RgbaPixels,
};

using MyArgTuple = Texpainter::FilterGraph::InArgTuple<types>;

constexpr auto args = MyArgTuple{};

static_assert(sizeof(MyArgTuple) == 3 * sizeof(void*));
static_assert(std::is_same_v<std::decay_t<decltype(std::get<0>(args))>,
                             Texpainter::FilterGraph::RealValue const*>);
static_assert(std::is_same_v<std::decay_t<decltype(std::get<1>(args))>,
                             Texpainter::FilterGraph::ComplexValue const*>);
static_assert(std::is_same_v<std::decay_t<decltype(std::get<2>(args))>,
                             Texpainter::PixelStore::RgbaValue const*>);
static_assert(std::tuple_size_v<MyArgTuple> == std::size(types));

namespace Testcases
{
	void texpainterFilterGraphArgTupleGetValues()
	{
		Texpainter::FilterGraph::RealValue x;
		Texpainter::FilterGraph::ComplexValue y;
		Texpainter::PixelStore::RgbaValue z;
		MyArgTuple args{&x, &y, &z};

		assert(std::get<0>(std::as_const(args)) == &x);
		assert(std::get<1>(std::as_const(args)) == &y);
		assert(std::get<2>(std::as_const(args)) == &z);
	}
}

int main()
{
	Testcases::texpainterFilterGraphArgTupleGetValues();
	return 0;
}