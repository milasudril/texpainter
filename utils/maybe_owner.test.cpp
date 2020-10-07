//@	{
//@	 "targets":[{"name":"maybe_owner.test", "type":"application", "autorun":1}]
//@	}

#include "./maybe_owner.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterMaybeOwnerInit()
	{
		Texpainter::MaybeOwner<int> i_am_an_owner{531};
		assert(i_am_an_owner.isOwner());

		int val = 5;
		Texpainter::MaybeOwner<int> i_am_not_an_owner{val};
		assert(!i_am_not_an_owner.isOwner());
		val = 6;
		assert(i_am_not_an_owner.get() == 6);
	}
}

int main()
{
	Testcases::texpainterMaybeOwnerInit();
	return 0;
}