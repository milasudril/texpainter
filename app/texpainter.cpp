//@	{"targets":[{"name":"texpainter","type":"application", "pkgconfig_libs":["gtk+-3.0"]}]}

#include "./palette_editor.hpp"
#include "./surface_creator.hpp"

#include "model/document.hpp"
#include "ui/window.hpp"
#include "ui/image_view.hpp"
#include "ui/palette_view.hpp"
#include "ui/extended_color_picker.hpp"
#include "ui/box.hpp"
#include "ui/color_picker_sidepanel.hpp"
#include "ui/dialog.hpp"
#include "ui/toolbar.hpp"

#include <gtk/gtk.h>

#include <functional>
#include <string>

enum class MenuAction : int
{
	GenNoise,
	GenCracks,
	GenVoronoi
};

constexpr auto end(Texpainter::Empty<MenuAction>)
{
	return static_cast<MenuAction>(static_cast<int>(MenuAction::GenVoronoi) + 1);
}

template<MenuAction action>
struct MenuActionTraits;

template<>
struct MenuActionTraits<MenuAction::GenNoise>
{
	using type = Texpainter::Ui::Button;
	static constexpr char const* name = "Gen. noise";
};

template<>
struct MenuActionTraits<MenuAction::GenCracks>
{
	using type = Texpainter::Ui::Button;
	static constexpr char const* name = "Gen. cracks";
};

template<>
struct MenuActionTraits<MenuAction::GenVoronoi>
{
	using type = Texpainter::Ui::Button;
	static constexpr char const* name = "Gen. voronoi";
};


struct MyCallback
{
	template<int>
	void onClose(Texpainter::Ui::Window&)
	{
		Texpainter::Ui::Window::terminateApp();
	}

	template<int>
	void onKeyDown(Texpainter::Ui::Window&, int scancode)
	{
		printf("(%d ", scancode);
		fflush(stdout);
	}

	template<int>
	void onKeyUp(Texpainter::Ui::Window&, int scancode)
	{
		printf("%d) ", scancode);
		fflush(stdout);
	}

	template<int>
	void onMouseDown(Texpainter::Ui::ImageView& event_source,
	                 Texpainter::vec2_t pos_window,
	                 Texpainter::vec2_t pos_screen,
	                 int button)
	{
		m_button_mask |= 1 << button;
		if(m_button_mask & (1 << 1))
		{
			auto& img = r_doc.get().image();
			img(static_cast<uint32_t>(pos_window[0]) % img.width(),
			    static_cast<uint32_t>(pos_window[1]) % img.height()) = selectedColor(r_doc);
			event_source.update();
		}
	}

	template<int>
	void onMouseUp(Texpainter::Ui::ImageView& event_source,
	               Texpainter::vec2_t pos_window,
	               Texpainter::vec2_t,
	               int button)
	{
		m_button_mask &= ~(1 << button);
	}

	template<int>
	void onMouseMove(Texpainter::Ui::ImageView& event_source,
	                 Texpainter::vec2_t pos_window,
	                 Texpainter::vec2_t)
	{
		if(m_button_mask & (1 << 1))
		{
			auto& img = r_doc.get().image();
			img(static_cast<uint32_t>(pos_window[0]) % img.width(),
			    static_cast<uint32_t>(pos_window[1]) % img.height()) = selectedColor(r_doc);
			event_source.update();
		}
	}

	template<int>
	void onClicked(Texpainter::Ui::Button& button)
	{
		button.state(false);
	}

	template<int>
	void onChanged(Texpainter::PaletteEditor& pal_editor)
	{
		r_doc.get().palette() = pal_editor.selectedPalette();
		r_doc.get().selectedColorIndex(pal_editor.selectedColorIndex());
	}

	template<MenuAction>
	void onClicked(Texpainter::Ui::Button& source)
	{
		source.state(false);
	}


	std::reference_wrapper<Texpainter::Model::Document> r_doc;
	Texpainter::Ui::Window* r_mainwin;
	uint32_t m_button_mask{};
};

int main(int argc, char* argv[])
{
	gtk_disable_setlocale();
	gtk_init(&argc, &argv);

	Texpainter::Model::Document doc;
	MyCallback cb{doc};

	Texpainter::Generators::SurfaceGenerator generator;
	doc.image() =
	   generator.filters(Texpainter::Generators::FilterGraph::None)
	      .cutoffFrequency(Texpainter::SpatialFrequency{Texpainter::vec2_t{1.0 / 128.0, 1.0 / 64.0}})
	      .orientation(Texpainter::Angle{0.083333, Texpainter::Angle::Turns{}})(
	         Texpainter::Size2d{512, 512});

	Texpainter::Ui::Window mainwin{"Texpainter"};
	mainwin.defaultSize(Texpainter::Geom::Dimension{}.width(800).height(600));
	Texpainter::Ui::Box box_outer{mainwin, Texpainter::Ui::Box::Orientation::Vertical};

	Texpainter::Ui::LabeledInput<Texpainter::PaletteEditor> pal_editor{
	   box_outer, Texpainter::Ui::Box::Orientation::Horizontal, "Palettes: "};
	pal_editor.inputField().eventHandler<0>(cb);

	box_outer.homogenous(false);
	Texpainter::Ui::Toolbar<MenuAction, MenuActionTraits, MyCallback> toolbar{box_outer,
	                                                              Texpainter::Ui::Box::Orientation::Horizontal, cb};
	cb.r_mainwin = &mainwin;


	box_outer.insertMode(
	   Texpainter::Ui::Box::InsertMode{4, Texpainter::Ui::Box::Expand | Texpainter::Ui::Box::Fill});
	/*	Texpainter::Ui::ImageView imgview{box_outer};
	 box_outer.insertMode(Texpainter::Ui::Box::InsertMode{4, 0});
	 imgview.image(std::as_const(doc.image())).eventHandler<0>(cb);*/

	Texpainter::SurfaceCreator surf{box_outer};


	//	box_outer.insertMode(Texpainter::Ui::Box::InsertMode{4, 0});


	mainwin.eventHandler<0>(cb).show();
	gtk_main();
}
