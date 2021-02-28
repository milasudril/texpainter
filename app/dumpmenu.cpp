//@	{"targets":[{"name":"dumpmenu","type":"application"}]}

#include "./main_menu.hpp"

#include "libenum/enum.hpp"

int main()
{
	Enum::forEachEnumItem<Texpainter::MainMenuItem>([](auto id) {
		printf("## %s\n\n%s\n\n",
		       Texpainter::MainMenuItemTraits<id.value>::displayName(),
		       Texpainter::MainMenuItemTraits<id.value>::description());
		using T      = Texpainter::MainMenuItemTraits<id.value>;
		using Action = typename T::Action;
		printf("| | |\n|-|-|\n");
		Enum::forEachEnumItem<Action>([id](auto id_inner) {
			using U = T::template Traits<id_inner.value>;
			printf("|%s|%s|\n", U::displayName(), U::description());
		});
		puts("\n");
	});
	return 0;
}