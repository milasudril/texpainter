//@	{
//@	 "targets":[{"name":"compositor_input.test", "type":"application", "autorun":1}]
//@	}

#include "./compositor_input.hpp"

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include <cassert>
#include <string>

namespace
{
	struct Dummy
	{
		int val;
	};

	class MyImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<Texpainter::FilterGraph::PortInfo, 0> InputPorts{};
			static constexpr std::array<Texpainter::FilterGraph::PortInfo, 0> OutputPorts{};
			static constexpr std::array<Texpainter::FilterGraph::ParamName, 0> ParamNames{};
		};

		explicit MyImageProcessor(std::string&& name): m_name{std::move(name)} {}

		void operator()(Texpainter::FilterGraph::ImgProcArg<InterfaceDescriptor> const&) const {}

		Texpainter::FilterGraph::ParamValue get(Texpainter::FilterGraph::ParamName) const
		{
			return Texpainter::FilterGraph::ParamValue{0.0};
		}

		void set(Texpainter::FilterGraph::ParamName, Texpainter::FilterGraph::ParamValue) {}

		std::span<Texpainter::FilterGraph::ParamValue const> paramValues() const
		{
			return std::span<Texpainter::FilterGraph::ParamValue const>{};
		}

		char const* name() const { return m_name.c_str(); }

		static constexpr auto id()
		{
			return Texpainter::FilterGraph::ImageProcessorId{"66bc12285d8881c91ff74fcde23e992f"};
		}

	private:
		std::string m_name;
	};

	struct Env
	{
		Texpainter::Model::Compositor compositor;
		std::map<Texpainter::Model::ItemName, Texpainter::Model::Compositor::NodeItem> nodes;
	};
}

namespace Texpainter::Model::detail
{
	template<>
	struct ImageProcessor<WithStatus<Dummy>>
	{
		using type = MyImageProcessor;
	};
}

namespace Testcases
{
	void modelCompositorInputManagerInitialState()
	{
		Env env;
		Texpainter::Model::CompositorInputManager<Dummy> obj;
		assert(std::as_const(obj).get(std::type_identity<Dummy>{}).size() == 0);

		auto const name = Texpainter::Model::ItemName{"Foo"};

		assert(!(std::as_const(obj).get(std::type_identity<Dummy>{}, name)));
		assert(!(obj.erase(std::type_identity<Dummy>{}, name, env.compositor, env.nodes)));
		assert(!obj.modify([](Dummy&) noexcept { return true; }, name));
	}

	void modelCompositorInputManagerInsertAndErase()
	{
		Env env;
		auto const node_count_init = env.compositor.nodeCount();
		Texpainter::Model::CompositorInputManager<Dummy> obj;

		auto const name = Texpainter::Model::ItemName{"Foo"};
		assert(obj.insert(name, Dummy{}, env.compositor, env.nodes) != nullptr);
		assert(obj.erase(std::type_identity<Dummy>{}, name, env.compositor, env.nodes));

		assert(!(std::as_const(obj).get(std::type_identity<Dummy>{}, name)));
		assert(!(obj.erase(std::type_identity<Dummy>{}, name, env.compositor, env.nodes)));
		assert(!obj.modify([](Dummy&) noexcept { return true; }, name));
		assert(env.nodes.size() == 0);
		assert(env.compositor.nodeCount() == node_count_init);
	}

	void modelCompositorInputManagerInsert()
	{
		Env env;
		auto const node_count_init = env.compositor.nodeCount();
		Texpainter::Model::CompositorInputManager<Dummy> obj;

		auto const name         = Texpainter::Model::ItemName{"Foo"};
		constexpr auto val_init = 123;
		auto res                = obj.insert(name, Dummy{val_init}, env.compositor, env.nodes);

		assert(env.compositor.nodeCount() == node_count_init + 1);
		assert(env.nodes.size() == 1);
		assert(res->source.get().val == val_init);
		assert(strcmp(res->processor.get().name(), "Foo") == 0);
		assert(res->processor.get().id() == MyImageProcessor::id());

		auto node = env.nodes.find(name);
		{
			assert(node != std::end(env.nodes));
			assert(node->first == name);
			assert(&node->second.second.get().processor() == &res->processor.get());
			assert(env.compositor.node(node->second.first) == &node->second.second.get());
		}

		{
			assert(obj.get(std::type_identity<Dummy>{}).size() == 1);
			auto const item = std::begin(obj.get(std::type_identity<Dummy>{}));
			assert(item->first == name);
			assert(item->second.source.get().val == val_init);
			assert(strcmp(item->second.processor.get().name(), "Foo") == 0);
			assert(&node->second.second.get().processor() == &item->second.processor.get());
		}

		assert(obj.modify(
		    [](Dummy& obj) noexcept {
			    ++obj.val;
			    return true;
		    },
		    name));
		assert(res->source.get().val == val_init + 1);
	}
}

int main()
{
	Testcases::modelCompositorInputManagerInitialState();
	Testcases::modelCompositorInputManagerInsertAndErase();
	Testcases::modelCompositorInputManagerInsert();
	return 0;
}