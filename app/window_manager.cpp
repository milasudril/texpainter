//@	{
//@	 "targets":[{"name":"window_manager.o","type":"object"}]
//@	}

#include "./window_manager.hpp"

void Texpainter::App::WindowManager::resetWindowPositions()
{
	auto const screen_size = Ui::Context::get().primaryWorkspaceSize();
	vec2_t const size_vec{static_cast<double>(screen_size.width()),
	                      static_cast<double>(screen_size.height())};
	vec2_t const size_vec_upper = vec2_t{0.5, 0.625} * size_vec;
	vec2_t const size_vec_lower = vec2_t{1.0, 1.0 - 0.625} * size_vec;

	auto const size_quarter =
	    Size2d{static_cast<uint32_t>(size_vec_upper[0]), static_cast<uint32_t>(size_vec_upper[1])};
	auto const size_half =
	    Size2d{static_cast<uint32_t>(size_vec_lower[0]), static_cast<uint32_t>(size_vec_lower[1])};

	if(auto img_editor = m_windows.get<WindowType::ImageEditor>().get(); img_editor != nullptr)
	{ img_editor->window().resize(size_quarter).move(Ui::ScreenCoordinates{0.0, 0.0}); }

	if(auto doc_previewer = m_windows.get<WindowType::DocumentPreviewer>().get();
	   doc_previewer != nullptr)
	{
		doc_previewer->window()
		    .resize(size_quarter)
		    .move(Ui::ScreenCoordinates{0.0, 0.0} + vec2_t{0.5, 0.0} * size_vec);
	}

	if(auto compositor = m_windows.get<WindowType::Compositor>().get(); compositor != nullptr)
	{
		compositor->window().resize(size_half).move(Ui::ScreenCoordinates{0.0, 0.0}
		                                            + vec2_t{0.0, 0.625} * size_vec);
	}
}

Texpainter::Model::Windows Texpainter::App::WindowManager::windowInfo() const
{
	Model::Windows ret;

	if(auto editor = m_windows.get<WindowType::ImageEditor>().get(); editor != nullptr)
	{
		ret.image_editor.rect =
		    Model::WindowRectangle{editor->window().size(), editor->window().location().value()};
		ret.image_editor.visible = true;
	}

	if(auto compositor = m_windows.get<WindowType::Compositor>().get(); compositor != nullptr)
	{
		ret.compositor.rect    = Model::WindowRectangle{compositor->window().size(),
                                                     compositor->window().location().value()};
		ret.compositor.visible = true;
	}

	if(auto previewer = m_windows.get<WindowType::DocumentPreviewer>().get(); previewer != nullptr)
	{
		ret.document_previewer.rect = Model::WindowRectangle{
		    previewer->window().size(), previewer->window().location().value()};
		ret.compositor.visible = true;
	}

	return ret;
}