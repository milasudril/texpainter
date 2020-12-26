//@	{"targets":[{"name":"texpainter","type":"application"}]}

#include "./window_manager.hpp"
#include "ui/context.hpp"

int main()
{
	auto& context = Texpainter::Ui::Context::get();
	Texpainter::App::WindowManager wm;
	context.run();
}
