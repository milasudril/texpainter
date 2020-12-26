//@	{"targets":[{"name":"texpainter","type":"application"}]}

#include "./document_editor.hpp"
#include "ui/context.hpp"

int main()
{
	auto& context = Texpainter::Ui::Context::get();
	Texpainter::App::WindowManager wm;
	context.run();
}
