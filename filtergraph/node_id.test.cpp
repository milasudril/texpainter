//@	{
//@	 "targets":[{"name":"node_id.test", "type":"application", "autorun":1}]
//@	}

#include "./node_id.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterNodeIdToJson()
	{
		nlohmann::json obj;
		to_json(obj, Texpainter::FilterGraph::InvalidNodeId);
		auto buffer = to_string(obj);
		assert(buffer == "18446744073709551615");
	}
}

int main()
{
	Testcases::texpainterNodeIdToJson();
	return 0;
}