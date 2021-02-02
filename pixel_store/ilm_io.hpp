//@	{
//@	 "targets":[{"name":"ilm_io.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_PIXELSTORE_ILMIO_HPP
#define TEXPAINTER_PIXELSTORE_ILMIO_HPP

#include <OpenEXR/ImfIO.h>

#include <span>

namespace Texpainter::PixelStore
{
	namespace detail
	{
		template<class T>
		void write(T& handle, char const* buffer, int n)
		{
			(void)write(handle, std::as_bytes(std::span{buffer, static_cast<size_t>(n)}));
		}

		template<class T>
		Imath::Int64 tellp(T& handle)
		{
			return tell(handle);
		}

		template<class T>
		void seekp(T& handle, Imath::Int64 pos)
		{
			(void)seek(handle, pos);
		}
	}

	template<class FileWriter>
	class IlmOutputAdapter: public Imf::OStream
	{
	public:
		explicit IlmOutputAdapter(FileWriter&& writer)
		    : Imf::OStream{"Dummy"}
		    , m_writer{std::move(writer)}
		{
		}

		void write(char const* buffer, int n) override { detail::write(m_writer, buffer, n); }

		Imath::Int64 tellp() override { return detail::tellp(m_writer); }

		void seekp(Imath::Int64 pos) override { detail::seekp(m_writer, pos); }

	private:
		FileWriter m_writer;
	};
}

#endif