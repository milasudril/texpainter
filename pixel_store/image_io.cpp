//@	{"targets":[{"name":"image_io.o", "type":"object", "pkgconfig_libs":["OpenImageIO"]}]}

#include "./image_io.hpp"

#include <OpenImageIO/imageio.h>

Texpainter::PixelStore::Image Texpainter::PixelStore::load(const char* filename)
{
	auto in = OIIO::ImageInput::open(filename);
	if(!in) { throw std::runtime_error{std::string{"Failed to load "} + filename}; }

	auto const& spec = in->spec();
	if(spec.width <= 0 || spec.height <= 0)
	{
		throw std::runtime_error{
		    std::string{"Unsupported image format. Width and hegiht must be greater than zero."}};
	}

	if(spec.width > 65535 || spec.height > 65535)
	{ throw std::runtime_error{std::string{"This image is too large."}}; }

#if 1
	if(auto attrib = spec.find_attribute("oiio:ColorSpace"); attrib != nullptr)
	{ printf("Hej %s\n", attrib->get<std::string>().c_str()); }
#endif

	Image ret{static_cast<uint32_t>(spec.width), static_cast<uint32_t>(spec.height)};

	if(spec.channelnames
	   == std::vector{std::string{"R"}, std::string{"G"}, std::string{"B"}, std::string{"A"}})
	{
		if(!in->read_image(OIIO::TypeDesc::FLOAT, ret.pixels().data()))
		{
			throw std::runtime_error{std::string{"Failed to load "} + filename + ": "
			                         + in->geterror()};
		}
	}
	else
	{
		throw std::runtime_error{"Unsupported channel layout"};
	}

	return ret;
}