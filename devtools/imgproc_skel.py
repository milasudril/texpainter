#!/usr/bin/env python3

import secrets
import string
import os


class ImgProc:
	def __init__(self, name, param_names):
		self.__name = name
		self.__param_names = param_names

	def name(self):
		return self.__name

	def paramNames(self):
		return self.__param_names

	pass


def stringEscape(string):
	result = ''
	for c in string:
		if c == '"' or c == '\\':
			result += '\\'
		result += c
	return result


def makeParamNameArray(param_names):
	escaped_names = []
	for name in param_names:
		escaped_names.append('"' + stringEscape(name) + '"')

	return 'static constexpr std::array<ParamName, %d> ParamNames{%s};' % (len(escaped_names),
																			','.join(escaped_names))


template = string.Template("""//@	{
//@	 "targets":[{"name":"$include_file", "type":"include"}]
//@	}

#ifndef $include_guard
#define $include_guard

#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/param_name.hpp"
#include "filtergraph/param_value.hpp"
$param_map_include
$user_includes
namespace $namespace_name
{
	using Texpainter::Size2d;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::PortType;
	$imported_types
	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			$input_ports
			$output_ports
			$param_names
		};


		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			$call_operator
		}

		$param_accessors

		static constexpr char const* name() { return "$processor_name"; }

		static constexpr auto id() { return ImageProcessorId{"$processor_id"}; }

	private:
		$param_map
		$user_state
	};
}

#endif
""")


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


imgproc = ImgProc(name='Foo Bar baz', param_names=['Param 1', 'Param \\2', 'Param "3"'])

main_substitutes = dict()
main_substitutes['processor_name'] = imgproc.name()
main_substitutes['include_file'] = makeIncludeFileName(main_substitutes['processor_name'])
main_substitutes['namespace_name'] = makeNamespaceName(main_substitutes['processor_name'])
main_substitutes['include_guard'] = makeIncludeGuard(main_substitutes['include_file'])
main_substitutes['param_map_include'] = '' if len(imgproc.paramNames()) == 0 \
                                           else '#include "filtergraph/param_map.hpp"'
main_substitutes['user_includes'] = ''
main_substitutes['imported_types'] = ''
main_substitutes['input_ports'] = 'static constexpr std::array<PortInfo, 0> InputPorts{};'
main_substitutes['output_ports'] = 'static constexpr std::array<PortInfo, 0> OutputPorts{};'
main_substitutes['param_names'] = makeParamNameArray(imgproc.paramNames())
main_substitutes['call_operator'] = ''
main_substitutes['param_accessors'] = ''
main_substitutes['processor_id'] = secrets.token_hex(16)
main_substitutes['param_map'] =  '' if len(imgproc.paramNames()) == 0 \
                                    else 'ParamMap<InterfaceDescriptor> params;'
main_substitutes['user_state'] = ''

print(template.substitute(main_substitutes))
