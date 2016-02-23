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
		self.parseSource()


	def parseSource(self):
		for source in self.source:
			lines = source.splitlines()
			for line in lines:
				print(line)
				tokens = line.split(" ")
				if tokens[0] == "uniform":
					self.uniforms[tokens[2]] = 0
					

f = open("vertex.glsl", "r")
vertexSource = f.read()
f.close()

f = open("frag.glsl", "r")
fragSource = f.read()
f.close()

shader = Shader(vertexSource, fragSource)
print(shader.uniforms)



