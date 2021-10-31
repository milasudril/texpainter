//@	{
//@	 "targets":[{"name":"preallocated_multiset.test", "type":"application", "autorun":1}]
//@	}

#include "./preallocated_multiset.hpp"

namespace Testcases
{
	void texpainterPreallocatedMultiset() { Texpainter::PreallocatedMultiset<double> vals{16}; }
}

int main() { return 0; }