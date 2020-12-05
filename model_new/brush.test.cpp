//@	{
//@	"targets":[{"name":"brush.test","type":"application", "autorun":1}]
//@	}

#include "./brush.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterBrushFunctionCall()
	{
		Texpainter::Model::BrushFunction brush{Texpainter::Model::BrushShape::Square};

		assert((brush(Texpainter::vec2_t{0.5, 0.5}, 1.0) == true));
		assert((brush(Texpainter::vec2_t{1.5, 0.5}, 1.0) == false));
		assert((brush(Texpainter::vec2_t{0.5, 1.5}, 1.0) == false));
		assert((brush(Texpainter::vec2_t{0.5, -1.5}, 1.0) == false));
	}
}

int main()
{
	Testcases::texpainterBrushFunctionCall();
	return 0;
}
