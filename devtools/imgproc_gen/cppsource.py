import secrets
import string
import os
import image_processor

ImgProc = image_processor.ImageProcessor


def makeIncludeFileName(name):
	return name.replace(' ', '_').lower() + '.hpp'


def makeNamespaceName(name):
	parts = name.split(' ')
	ret = []
	for item in parts:
		ret.append(item.capitalize())
	return ''.join(ret)


def splitall(path):
	allparts = []
	while 1:
		parts = os.path.split(path)
		if parts[0] == path:  # sentinel for absolute paths
			allparts.insert(0, parts[0])
			break
		elif parts[1] == path:  # sentinel for relative paths
			allparts.insert(0, parts[1])
			break
		else:
			path = parts[0]
		allparts.insert(0, parts[1])
	return allparts


def makeIncludeGuard(filename):
	cwd = os.path.split(os.getcwd())[1]
	path = splitall(filename)
	if path[0] == '.':
		path = path[1:]
	path = [cwd] + path

	parts = []
	for item in path:
		res = item.replace('_', '')
		res = res.replace('-', '')
		res = res.replace('.', '_')
		res = res.upper()
		parts.append(res)

	return '_'.join(parts)


def stringEscape(string):
	result = ''
	for c in string:
		if c == '"' or c == '\\':
			result += '\\'
		result += c
	return result


def makeParamMapInclude(params):
	if len(params) == 0:
		return ''
	else:
		return '#include "filtergraph/param_map.hpp"'


def makeImplDecl(params):
	if len(params) == 0:
		return 'void main(auto const& args);'
	else:
		return '''using Texpainter::Str;
		using Texpainter::FilterGraph::param;

		void main(auto const& args, auto const& params);'''


def makeCallOperator(params):
	if len(params) == 0:
		return 'impl::main(args);'
	else:
		return '''impl::main(args, params);'''


def makePortArray(name, ports):
	items = []
	for key, value in ports.items():
		items.append('PortInfo{PortType::%s, "%s"}' % (value, stringEscape(key)))

	return 'static constexpr std::array<PortInfo, %d> %s{{%s}};' % (len(items), name,
		', '.join(items))


def makeParamNameArray(param_names):
	escaped_names = []
	for name in param_names:
		escaped_names.append('"' + stringEscape(name) + '"')

	return 'static constexpr std::array<ParamName, %d> ParamNames{%s};' % (len(escaped_names),
		', '.join(escaped_names))


def makeDefaultCtor(params):
	if len(params) == 0:
		return ''
	else:
		params_init = []
		for key, value in sorted(params.items()):
			formatted_number = '%.17g' % value
			if not '.' in formatted_number:
				formatted_number += '.0'
			params_init.append('ParamValue{static_cast<float>(%s)}' % formatted_number)
		return '''explicit ImageProcessor():params{{%s}}
		{
		}''' % ', '.join(params_init)


def makeParamMapObject(param_names):
	if len(param_names) == 0:
		return ''
	else:
		return 'Texpainter::FilterGraph::ParamMap<InterfaceDescriptor> params;'


def makeParamAccessors(param_names):
	if len(param_names) == 0:
		return '''void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }'''
	else:
		return '''void set(ParamName name, ParamValue value)
		{
			if(auto ptr = params.find(name); ptr != nullptr) [[likely]]
			{
				*ptr = value;
			}
		}

		ParamValue get(ParamName name) const
		{
			auto ptr = params.find(name);
			return ptr != nullptr ? *ptr : ParamValue{0.0};
		}'''


template = string.Template("""//@	{
//@	 "targets":[{"name":"$include_file", "type":"include"}]
//@	}

#ifndef $include_guard
#define $include_guard

#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/img_proc_param.hpp"
#include "utils/vec_t.hpp"
#include "utils/angle.hpp"
#include "utils/rect.hpp"
#include "wavelength_conv/wavelength_to_rgba.hpp"
#include "pixel_store/image.hpp"

$param_map_include
#line $user_includes_start "$src_file"
$user_includes

namespace $namespace_name
{
	using Texpainter::Size2d;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcReleaseState;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::PortType;
	using Rng = Texpainter::DefaultRng::Engine;
	namespace WavelengthConv = Texpainter::WavelengthConv;

	namespace impl
	{
		using Texpainter::FilterGraph::input;
		using Texpainter::FilterGraph::output;
		using Texpainter::FilterGraph::RgbaValue;
		using Texpainter::FilterGraph::ComplexValue;
		using Texpainter::FilterGraph::RealValue;
		using Texpainter::FilterGraph::Palette;
		using Texpainter::FilterGraph::TopographyInfo;
		using Texpainter::FilterGraph::ImageCoordinates;
		using Texpainter::FilterGraph::SpawnSpot;
		using Texpainter::FilterGraph::PolylineSet;
		using Texpainter::FilterGraph::LineSegTree;
		using Texpainter::AxisAlignedBoundingBox;
		using Texpainter::vec2_t;
		using Texpainter::Angle;
		using Texpainter::PixelStore::Image;
		$impl_decl
	}

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			$input_ports
			$output_ports
			$param_names
		};

		$default_ctor

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			$call_operator
		}

		$param_accessors

		static constexpr char const* name() { return "$processor_name"; }

		static constexpr auto id() { return ImageProcessorId{"$processor_id"}; }

		static constexpr char const* description() { return "$description"; }

		static constexpr char const* category() { return "$category"; }

		static constexpr auto releaseState() { return ImgProcReleaseState::$release_state; }

	private:
		$param_map
	};
}

namespace $namespace_name::impl
{
#line $impl_start "$src_file"
$img_proc_body

}

#endif
""")


def makeCppSource(imgproc, src_file):
	main_substitutes = dict()
	main_substitutes['processor_name'] = imgproc.name()
	main_substitutes['include_file'] = makeIncludeFileName(imgproc.name())
	main_substitutes['namespace_name'] = makeNamespaceName(imgproc.name())
	main_substitutes['include_guard'] = makeIncludeGuard(main_substitutes['include_file'])
	main_substitutes['param_map_include'] = makeParamMapInclude(imgproc.params().keys())
	main_substitutes['user_includes'] = '\n'.join(imgproc.userIncludes())
	main_substitutes['impl_decl'] = makeImplDecl(imgproc.params())
	main_substitutes['input_ports'] = makePortArray('InputPorts', imgproc.inputPorts())
	main_substitutes['output_ports'] = makePortArray('OutputPorts', imgproc.outputPorts())
	main_substitutes['param_names'] = makeParamNameArray(imgproc.params().keys())
	main_substitutes['default_ctor'] = makeDefaultCtor(imgproc.params())
	main_substitutes['call_operator'] = makeCallOperator(imgproc.params())
	main_substitutes['param_accessors'] = makeParamAccessors(imgproc.params().keys())
	main_substitutes['processor_id'] = imgproc.processorId()
	main_substitutes['param_map'] = makeParamMapObject(imgproc.params().keys())
	main_substitutes['img_proc_body'] = imgproc.body()
	main_substitutes['description'] = stringEscape(imgproc.description())
	main_substitutes['category'] = stringEscape(imgproc.category())
	main_substitutes['impl_start'] = str(imgproc.implStart())
	main_substitutes['user_includes_start'] = str(imgproc.userIncludesStart())
	main_substitutes['src_file'] = src_file
	main_substitutes['release_state'] = imgproc.releaseState()

	return template.substitute(main_substitutes)
