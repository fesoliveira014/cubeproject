class Shader():
	def __init__(self, vertexSource, fragSource, geomSource = None):
		self.source = []
		if not vertexSource == None:
			self.source.append(vertexSource)
		if not fragSource == None:
			self.source.append(fragSource)
		if not geomSource == None:
			self.source.append(geomSource)
		self.uniforms = dict()
		self.structs = dict()
		self.parseSource()


	def parseSource(self):
		reading_struct = False
		current_struct = ""
		for source in self.source:
			lines = source.splitlines()
			for line in lines:
				#print(line)
				tokens = line.lstrip(" ").split(" ")
				if reading_struct:
					print(current_struct)
					print(tokens[0])
					if tokens[0] == "};":
						reading_struct = False
						current_struct = ""
					elif not tokens[0] is "":
						self.structs[current_struct].append(tokens[1][:-1])

				elif tokens[0] == "uniform":
					if tokens[1] in self.structs:
						for attribute in self.structs[tokens[1]]:
							self.uniforms[tokens[2][:-1] + "." + attribute] = 0
					else:
						if tokens[2][:-1] == ";":
							self.uniforms[tokens[2][:-1]] = 0
						else:
							self.uniforms[tokens[2]] = 0
				elif tokens[0] == "struct":
					self.structs[tokens[1]] = []
					reading_struct = True
					current_struct = tokens[1]


					

f = open("vertex.glsl", "r")
vertexSource = f.read()
f.close()

f = open("frag.glsl", "r")
fragSource = f.read()
f.close()

shader = Shader(vertexSource, fragSource)
print(shader.structs)
print(shader.uniforms)



