//@	{"targets":[{"name":"dumpmenu","type":"application"}]}

#include "./main_menu.hpp"

#include "libenum/enum.hpp"

int main()
{
	Enum::forEachEnumItem<Texpainter::MainMenuItem>([](auto id) {
		printf("## %s\n\n%s\n\n",
		       Texpainter::MainMenuItemTraits<id.value>::displayName(),
		       Texpainter::MainMenuItemTraits<id.value>::description());
	});
	return 0;
}