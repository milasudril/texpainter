//@	{"targets":[{"name":"datablock.test", "type":"application", "autorun":1}]}

#include "./datablock.hpp"

#include <vector>
#include <cassert>
#include <functional>
#include <cstring>

namespace
{
	struct Buffer
	{
		std::vector<std::byte> m_data;
		uint32_t m_n_elems;
	};

	void write(uint32_t val, Buffer& buff) { buff.m_n_elems = val; }

	uint32_t read(Texpainter::Empty<uint32_t>, Buffer const& buff) { return buff.m_n_elems; }

	template<Texpainter::Trivial T>
	void write(std::span<T> data, Buffer& buff)
	{
		buff.m_data.resize(sizeof(T) * std::size(data));
		memcpy(std::data(buff.m_data), std::data(data), sizeof(T) * std::size(data));
	}

	template<Texpainter::Trivial T>
	void read(std::span<T> data, Buffer const& buff)
	{
		memcpy(std::data(data), std::data(buff.m_data), sizeof(T) * std::size(data));
	}
}

namespace Testcases
{
	void texpainterDatablockWriteRead()
	{
		Texpainter::DataBlock<int> test{4};
		test.begin()[0] = 1;
		test.begin()[1] = 2;
		test.begin()[2] = 3;
		test.begin()[3] = 4;

		Buffer buffer{};
		write(std::as_const(test), std::ref(buffer));
		assert(buffer.m_n_elems == std::size(test));
		assert(buffer.m_data[0] == static_cast<std::byte>(0x01));
		assert(buffer.m_data[4] == static_cast<std::byte>(0x02));
		assert(buffer.m_data[8] == static_cast<std::byte>(0x03));
		assert(buffer.m_data[12] == static_cast<std::byte>(0x04));

		auto test_2 = read(Texpainter::Empty<decltype(test)>{}, std::ref(buffer));

		assert(test_2.size() == test.size());
		assert(memcmp(std::begin(test_2), std::begin(test), std::size(test) * sizeof(int)) == 0);
	}
}


int main()
{
	Testcases::texpainterDatablockWriteRead();
	return 0;
}