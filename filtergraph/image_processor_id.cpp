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

void Texpainter::FilterGraph::from_json(nlohmann::json const& obj, ImgProcReleaseState& val)
{
	std::string str{obj};
	if(str == "experimental")
	{
		val = ImgProcReleaseState::Experimental;
		return;
	}

	if(str == "stable")
	{
		val = ImgProcReleaseState::Stable;
		return;
	}

	if(str == "deprecated")
	{
		val = ImgProcReleaseState::Deprecated;
		return;
	}

	throw "Bad ImgProcReleaseState";
}

std::string Texpainter::FilterGraph::toString(ImageProcessorId const& id)
{
	return bytesToHex(id.data());
}