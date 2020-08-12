//@	{
//@	 "targets":[{"name":"argtuple.test", "type":"application", "autorun":1}]
//@	}

#include "./argtuple.hpp"

constexpr auto types = std::array<Texpainter::FilterGraph::PixelType, 3>{
    Texpainter::FilterGraph::PixelType::GrayscaleReal,
    Texpainter::FilterGraph::PixelType::GrayscaleComplex,
    Texpainter::FilterGraph::PixelType::RGBA,
};

using MyArgTuple = Texpainter::FilterGraph::ArgTuple<types>;

constexpr auto args = MyArgTuple{};

static_assert(sizeof(MyArgTuple) == 3 * sizeof(void*));
static_assert(std::is_same_v<decltype(Texpainter::FilterGraph::get<0>(args)), double const*>);
static_assert(
    std::is_same_v<decltype(Texpainter::FilterGraph::get<1>(args)), std::complex<double> const*>);
static_assert(std::is_same_v<decltype(Texpainter::FilterGraph::get<2>(args)),
                             Texpainter::PixelStore::Pixel const*>);

int main() { return 0; }