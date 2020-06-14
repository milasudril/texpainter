//@	{"targets":[{"name":"texpainter","type":"application", "autorun":0,"pkgconfig_libs":["gtk+-3.0"]}]}

#include "ui/window.hpp"
#include "ui/image_view.hpp"
// #include "ui/box.hpp"

#include <gtk/gtk.h>

#include <functional>

struct MyCallback
{
	template<int>
	void closing(Texpainter::Ui::Window&)
	{Texpainter::Ui::Window::terminateApp();}

	template<int>
	void onMouseDown(Texpainter::Ui::ImageView& event_source,
	                 Texpainter::vec2_t pos_window,
	                 Texpainter::vec2_t pos_screen,
	                 int button)
	{
		m_button_mask |= 1<<button;
		printf("%d %d\n", m_button_mask, button);
		if(m_button_mask & (1 << 1))
		{
			r_img.get().get(pos_window[0], pos_window[1]) = Texpainter::Model::white();
			event_source.update();
		}
	}

	template<int>
	void onMouseUp(Texpainter::Ui::ImageView& event_source,
	                 Texpainter::vec2_t pos_window,
	                 Texpainter::vec2_t,
	                 int button)
	{
		m_button_mask &= ~(1<<button);
		printf("%d %d\n", m_button_mask, button);
	}

	template<int>
	void onMouseMove(Texpainter::Ui::ImageView& event_source,
	                 Texpainter::vec2_t pos_window,
	                 Texpainter::vec2_t)
	{
		if(m_button_mask & (1<<1))
		{
			r_img.get().get(pos_window[0], pos_window[1]) = Texpainter::Model::white();
			event_source.update();
		}
	}



	std::reference_wrapper<Texpainter::Model::Image> r_img;
	uint32_t m_button_mask{};

};

int main(int argc, char* argv[])
{
	gtk_init(&argc, &argv);


	Texpainter::Ui::Window mainwin{"Texpainter"};
	mainwin.defaultSize(Texpainter::Geom::Dimension{}.width(800).height(500));


	Texpainter::Model::Image img{641, 480};
	MyCallback cb{img};

	Texpainter::Ui::ImageView imgview{mainwin};
	imgview.image(img).eventHandler<0>(cb);


//	Texpainter::Ui::Box box{mainwin, Texpainter::UiBox::Orientation::Vertical};

	mainwin.callback<0>(cb).show();
	gtk_main();
}