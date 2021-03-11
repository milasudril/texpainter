//@	{
//@	 "targets":[{"name":"image_processor_id.o", "type":"object"}]
//@	}

#include "./image_processor_id.hpp"

void Texpainter::FilterGraph::to_json(nlohmann::json& obj, ImgProcReleaseState val)
{
	switch(val)
	{
		case ImgProcReleaseState::Experimental: obj = "experimental"; return;
		case ImgProcReleaseState::Stable: obj = "stable"; return;
		case ImgProcReleaseState::Deprecated: obj = "deprecated"; return;
	}
}