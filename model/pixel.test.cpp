//@	{"targets":[{"name":"pixel.test", "type":"application", "autorun":1}]}

#include "pixel.hpp"

#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstring>

namespace
{
	struct Buffer
	{
		std::vector<float> m_data;
	};

	void write(Buffer& buffer, std::span<float const> vals)
	{
		printf("%p %zu\n", vals.data(), vals.size());
		std::copy(std::begin(vals), std::end(vals), std::back_inserter(buffer.m_data));
	}

	struct CallFree
	{
		void operator()(void* buffer)
		{ free(buffer);}
	};

	template<class T>
	struct DataBlock
	{

		template<class U>
		using pointer_wrapper = std::unique_ptr<U, CallFree>;

		pointer_wrapper<T> m_vals;
		size_t m_size;

		auto release()
		{ return m_vals.release(); }

		size_t size() const
		{ return m_size; }
	};

	DataBlock<float> read(Buffer const& buffer, Texpainter::Empty<float[]>)
	{
		DataBlock<float> ret;
		ret.m_size = buffer.m_data.size();
		auto const n_bytes = buffer.m_data.size()*sizeof(float);
		ret.m_vals.reset(reinterpret_cast<float*>(malloc(n_bytes)));
		memcpy(ret.m_vals.get(), buffer.m_data.data(), n_bytes);
		return ret;
	}
}

namespace Testcases
{
	static_assert(std::is_trivial_v<Texpainter::Model::Pixel>);
	void texpainterModelPixelsWriteRead()
	{
		std::vector<Texpainter::Model::Pixel> pixels{Texpainter::Model::red(), Texpainter::Model::green(), Texpainter::Model::blue()};

		Buffer buffer;
		write(pixels, std::ref(buffer));

		std::vector<float> res_expected{1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f};

		assert(res_expected == buffer.m_data);

		auto read_back = read(Texpainter::Empty<DataBlock<Texpainter::Model::Pixel>>{}, std::cref(buffer));

		assert(read_back.m_size == pixels.size());
		assert(memcmp(read_back.m_vals.get(), pixels.data(), sizeof(Texpainter::Model::Pixel) * pixels.size()) == 0);
	}
}

int main()
{
	Testcases::texpainterModelPixelsWriteRead();
	return 0;
}