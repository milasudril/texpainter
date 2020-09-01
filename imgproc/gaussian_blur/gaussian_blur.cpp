//@	{
//@	 "targets":[{"name":"gaussian_blur.o", "type":"object"}]
//@	}

#include "./gaussian_blur.hpp"

void GaussianBlur::ImageProcessor::operator()(ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size = args.size();
#if 0
	if(size != m_size_prev || m_kernel_dirty) [[unlikely]]
	{
		m_kernel = generateKernel(size, m_r_x, m_r_y);
	}

#endif
	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			args.output<0>(col, row) = 0;
		}
	}
}