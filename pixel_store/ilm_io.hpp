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
		void write(void* handle, char const* buffer, int n)
		{
			(void)write(*static_cast<T*>(handle),
			            std::as_bytes(std::span{buffer, static_cast<size_t>(n)}));
		}

		template<class T>
		Imath::Int64 tellp(void* handle)
		{
			return tell(*static_cast<T*>(handle));
		}

		template<class T>
		void seekp(void* handle, Imath::Int64 pos)
		{
			(void)seek(*static_cast<T*>(handle), pos);
		}
	}

	class IlmOutputAdapter: public Imf::OStream
	{
	public:
		template<class T>
		explicit IlmOutputAdapter(std::reference_wrapper<T> ref)
		    : Imf::OStream{"Dummy"}
		    , m_handle{&ref.get()}
		    , m_write{detail::write<T>}
		    , m_tellp{detail::tellp<T>}
		    , m_seekp{detail::seekp<T>}
		{
		}

		void write(char const* buffer, int n) override { m_write(m_handle, buffer, n); }

		Imath::Int64 tellp() override { return m_tellp(m_handle); }

		void seekp(Imath::Int64 pos) override { m_seekp(m_handle, pos); }

	private:
		void* m_handle;
		void (*m_write)(void*, char const*, int);
		Imath::Int64 (*m_tellp)(void*);
		void (*m_seekp)(void*, Imath::Int64);
	};
}

#endif