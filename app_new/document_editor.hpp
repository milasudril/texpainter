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

		auto& menu() { return m_menu; }

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
		struct AppWindowTypeTraits;

		template<>
		struct AppWindowTypeTraits<AppWindowType::FilterGraphEditor>
		{
			using type = std::unique_ptr<FilterGraphEditorWindow>;
			static constexpr char const* name() { return "Texpainter: Filter graph"; }
		};

		template<>
		struct AppWindowTypeTraits<AppWindowType::ImageEditor>
		{
			using type = std::unique_ptr<ImageEditorWindow>;
			static constexpr char const* name() { return "Texpainter: Image editor"; }
		};

		template<>
		struct AppWindowTypeTraits<AppWindowType::Output>
		{
			using type = std::unique_ptr<OutputWindow>;
			static constexpr char const* name() { return "Texpainter: Output"; }
		};
	}

	class DocumentEditor
	{
		template<AppWindowType id, class... Args>
		auto createWindow(Args&&... args)
		{
			using T  = typename detail::AppWindowTypeTraits<id>::type::element_type;
			auto ret = std::make_unique<T>(detail::AppWindowTypeTraits<id>::name(),
			                               std::forward<Args>(args)...);
			ret->window().template eventHandler<id>(*this);
			return ret;
		}

	public:
		DocumentEditor(): m_document{Size2d{512, 512}}, m_window_count{3}
		{
			m_windows.get<AppWindowType::FilterGraphEditor>() =
			    createWindow<AppWindowType::FilterGraphEditor>(m_document);
			m_windows.get<AppWindowType::ImageEditor>() =
			    createWindow<AppWindowType::ImageEditor>(m_document);

			m_windows.get<AppWindowType::Output>() = createWindow<AppWindowType::Output>();

			Enum::forEachEnumItem<AppWindowType>(
			    [this](auto item) { m_windows.get<item.value>()->menu().eventHandler(*this); });
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
				m_document.nodeLocations(
				    m_windows.get<AppWindowType::FilterGraphEditor>()->widget().nodeLocations());
			}
			m_windows.get<id>().reset();
			if(m_window_count == 0) { gtk_main_quit(); }
		}

		template<AppWindowType, class Source>
		void handleException(char const* msg, Source&)
		{
			fprintf(stderr, "Error: %s\n", msg);
		}

		template<auto>
		void handleException(char const* msg, Ui::MenuItem&)
		{
			fprintf(stderr, "Error: %s\n", msg);
		}

		template<auto id>
		requires(!std::same_as<decltype(id), AppWindowType>) void onActivated(Ui::MenuItem&)
		{
			fprintf(stderr, "Unimplemented action\n");
		}

		template<AppWindowType item>
		void onActivated(Ui::MenuItem&)
		{
			if(m_windows.get<item>() == nullptr)
			{
				if constexpr(item != AppWindowType::Output)
				{ m_windows.get<item>() = createWindow<item>(m_document); }
				else
				{
					m_windows.get<item>() = createWindow<item>();
				}
				++m_window_count;
			}
			m_windows.get<item>()->window().show();
		}

	private:
		Model::Document m_document;
		Enum::Tuple<AppWindowType, detail::AppWindowTypeTraits> m_windows;

		size_t m_window_count;
	};

	template<>
	inline void DocumentEditor::onActivated<AppAction::Quit>(Ui::MenuItem&)
	{
		gtk_main_quit();
	}

	template<>
	inline void DocumentEditor::onActivated<ImageAction::New>(Ui::MenuItem&)
	{
		// TODO: Show image creation dialog
		m_document.insert(Model::ItemName{"test"}, PixelStore::Image{256, 256});
		// TODO: Set current image to the newly inserted one
		m_windows.get<AppWindowType::ImageEditor>()->widget().refresh();
		// TODO: Update other windows
		m_windows.get<AppWindowType::ImageEditor>()->window().show();
	}
}

#endif