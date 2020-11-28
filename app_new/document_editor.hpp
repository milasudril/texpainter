//@	{"targets":[{"name":"document_editor.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_DOCUMENTEDITOR_HPP
#define TEXPAINTER_APP_DOCUMENTEDITOR_HPP

#include "./image_editor.hpp"
#include "./filter_graph_editor.hpp"
#include "./main_menu.hpp"

#include "model_new/document.hpp"

#include "ui/window.hpp"
#include "ui/image_view.hpp"

#include <gtk/gtk.h>

namespace Texpainter::App
{
	template<class Widget>
	class WidgetWithWindow
	{
	public:
		template<class... Args>
		explicit WidgetWithWindow(char const* title, Args&&... args)
		    : m_window{title}
		    , m_root{m_window, Ui::Box::Orientation::Vertical}
		    , m_menu{m_root}
		    , m_widget{m_root.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand}),
		               std::forward<Args>(args)...}
		{
			m_window.resize(Size2d{800, 500}).show();
		}

		auto& window() { return m_window; }

		auto& widget() { return m_widget; }

	private:
		Ui::Window m_window;
		Ui::Box m_root;
		Ui::MenuBuilder<MainMenuItem, MainMenuItemTraits> m_menu;
		Widget m_widget;
	};

	namespace detail
	{
		using FilterGraphEditorWindow = WidgetWithWindow<FilterGraphEditor>;
		using ImageEditorWindow       = WidgetWithWindow<ImageEditor>;
		using OutputWindow            = WidgetWithWindow<Ui::ImageView>;

		template<AppWindowType>
		struct AppWindowTypeToType;

		template<>
		struct AppWindowTypeToType<AppWindowType::FilterGraphEditor>
		{
			using type = std::unique_ptr<FilterGraphEditorWindow>;
		};

		template<>
		struct AppWindowTypeToType<AppWindowType::ImageEditor>
		{
			using type = std::unique_ptr<ImageEditorWindow>;
		};

		template<>
		struct AppWindowTypeToType<AppWindowType::Output>
		{
			using type = std::unique_ptr<OutputWindow>;
		};
	}

	class DocumentEditor
	{
		template<AppWindowType id, class... Args>
		auto createWindow(Args&&... args)
		{
			using T  = typename detail::AppWindowTypeToType<id>::type::element_type;
			auto ret = std::make_unique<T>(std::forward<Args>(args)...);
			ret->window().template eventHandler<id>(*this);
			return ret;
		}

	public:
		DocumentEditor(): m_document{Size2d{512, 512}}, m_window_count{3}
		{
			m_windows.get<AppWindowType::FilterGraphEditor>() =
			    createWindow<AppWindowType::FilterGraphEditor>("Texpainter: Filter graph",
			                                                       m_document);
			m_windows.get<AppWindowType::ImageEditor>() =
			    createWindow<AppWindowType::ImageEditor>("Texpainter: ImageEditor", m_document);

			m_windows.get<AppWindowType::Output>() =
			    createWindow<AppWindowType::Output>("Texpainter: Output");
		}

		template<AppWindowType>
		void onKeyUp(Ui::Window&, Ui::Scancode)
		{
		}

		template<AppWindowType>
		void onKeyDown(Ui::Window&, Ui::Scancode)
		{
		}

		template<AppWindowType id>
		void onClose(Ui::Window&)
		{
			--m_window_count;
			if constexpr(id == AppWindowType::FilterGraphEditor)
			{
				m_document.nodeLocations(m_windows.get<AppWindowType::FilterGraphEditor>()
				                             ->widget()
				                             .nodeLocations());
			}
			m_windows.get<id>().reset();
			if(m_window_count == 0) { gtk_main_quit(); }
		}

		template<AppWindowType>
		void handleException(char const*, Ui::Window&)
		{
		}

	private:
		Model::Document m_document;
		Enum::Tuple<AppWindowType, detail::AppWindowTypeToType> m_windows;

		size_t m_window_count;
	};
}

#endif