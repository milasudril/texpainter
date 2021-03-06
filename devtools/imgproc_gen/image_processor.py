class ImageProcessor:
	def __init__(self,
		name,
		body,
		processor_id,
		input_ports=dict(),
		output_ports=dict(),
		params=dict(),
		user_includes=[],
		description='',
		category='',
		impl_start=0,
		user_includes_start=0,
		release_state='Experimental'):
		self.__name = name
		self.__body = body
		self.__user_includes = user_includes
		self.__input_ports = input_ports
		self.__output_ports = output_ports
		self.__params = params
		self.__processor_id = processor_id
		self.__description = description
		self.__category = category
		self.__impl_start = impl_start
		self.__user_includes_start = user_includes_start
		self.__release_state = release_state

	def name(self):
		return self.__name

	def params(self):
		return self.__params

	def inputPorts(self):
		return self.__input_ports

	def outputPorts(self):
		return self.__output_ports

	def userIncludes(self):
		return self.__user_includes

	def body(self):
		return self.__body

	def processorId(self):
		return self.__processor_id

	def description(self):
		return self.__description

	def category(self):
		return self.__category

	def implStart(self):
		return self.__impl_start

	def userIncludesStart(self):
		return self.__user_includes_start

	def releaseState(self):
		return self.__release_state
