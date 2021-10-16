//@	{"targets":[{"name":"rgba_value.test", "type":"application", "autorun":1}]}

#include "./rgba_value.hpp"

#include <type_traits>

namespace Testcases
{
	static_assert(std::is_trivial_v<Texpainter::PixelTypes::RgbaValue>);
}

int main() { return 0; }