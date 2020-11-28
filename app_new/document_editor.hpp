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

		enum class ControlId : int
		{
			FilterGraphEditor,
			ImageEditor,
			OutputWindow
		};

		constexpr auto begin(Enum::Empty<ControlId>) { return ControlId::FilterGraphEditor; }

		constexpr auto end(Enum::Empty<ControlId>)
		{
			return static_cast<ControlId>(static_cast<int>(ControlId::OutputWindow) + 1);
		}

		template<ControlId>
		struct ControlIdToType;

		template<>
		struct ControlIdToType<ControlId::FilterGraphEditor>
		{
			using type = std::unique_ptr<FilterGraphEditorWindow>;
		};

		template<>
		struct ControlIdToType<ControlId::ImageEditor>
		{
			using type = std::unique_ptr<ImageEditorWindow>;
		};

		template<>
		struct ControlIdToType<ControlId::OutputWindow>
		{
			using type = std::unique_ptr<OutputWindow>;
		};
	}

	class DocumentEditor
	{
		template<detail::ControlId id, class... Args>
		auto createWindow(Args&&... args)
		{
			using T  = typename detail::ControlIdToType<id>::type::element_type;
			auto ret = std::make_unique<T>(std::forward<Args>(args)...);
			ret->window().template eventHandler<id>(*this);
			return ret;
		}

	public:
		DocumentEditor(): m_document{Size2d{512, 512}}, m_window_count{3}
		{
			m_windows.get<detail::ControlId::FilterGraphEditor>() =
			    createWindow<detail::ControlId::FilterGraphEditor>("Texpainter: Filter graph",
			                                                       m_document);
			m_windows.get<detail::ControlId::ImageEditor>() =
			    createWindow<detail::ControlId::ImageEditor>("Texpainter: ImageEditor", m_document);

			m_windows.get<detail::ControlId::OutputWindow>() =
			    createWindow<detail::ControlId::OutputWindow>("Texpainter: Output");
		}

		template<detail::ControlId>
		void onKeyUp(Ui::Window&, Ui::Scancode)
		{
		}

		template<detail::ControlId>
		void onKeyDown(Ui::Window&, Ui::Scancode)
		{
		}

		template<detail::ControlId id>
		void onClose(Ui::Window&)
		{
			--m_window_count;
			if constexpr(id == detail::ControlId::FilterGraphEditor)
			{
				m_document.nodeLocations(m_windows.get<detail::ControlId::FilterGraphEditor>()
				                             ->widget()
				                             .nodeLocations());
			}
			m_windows.get<id>().reset();
			if(m_window_count == 0) { gtk_main_quit(); }
		}

		template<detail::ControlId>
		void handleException(char const*, Ui::Window&)
		{
		}

	private:
		Model::Document m_document;
		Enum::Tuple<detail::ControlId, detail::ControlIdToType> m_windows;

		size_t m_window_count;
	};
}

#endif