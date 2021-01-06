//@	{"targets":[{"name":"pixel.test", "type":"application", "autorun":1}]}

#include "./pixel.hpp"

#include <type_traits>

namespace Testcases
{
	static_assert(std::is_trivial_v<Texpainter::PixelStore::Pixel>);
}

int main() { return 0; }