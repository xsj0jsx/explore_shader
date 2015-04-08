#include <exception>
#include <stdexcept>
#include <string>
#include <glload/gl_3_3.h>
#include <glload/gl_load.hpp>
#include <GL/freeglut.h>
#include <fstream>
#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/core/func_common.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <float.h>
#include <algorithm>
GLuint BuildShader(GLenum eShaderType, const std::string &shaderText)
{
	GLuint shader = glCreateShader(eShaderType);
	const char *strFileData = shaderText.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		//With ARB_debug_output, we already get the info log on compile failure.
		//if(!glext_ARB_debug_output)
		{
			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar *strInfoLog = new GLchar[infoLogLength + 1];
			glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

			const char *strShaderType = NULL;
			switch(eShaderType)
			{
			case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
			case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
			case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
			}

			printf("Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
			delete[] strInfoLog;
		}

		throw std::runtime_error("Compile failure in shader.");
	}

	return shader;
}
std::string file2string(const std::string& fname)
{
	using std::ifstream;
    std::cout << "file:" << fname << " tostring\n";
	ifstream file(fname.c_str());
	assert(file);
	file.seekg(0, file.end);
	auto length = file.tellg();
	file.seekg(0, file.beg);
	std::string txt(length, ' ');
	file.read(&txt[0], length);
	return txt;
}
GLuint buildProgrameFromFile(const std::map<GLuint, std::string> &type_fname)
{
	GLuint program = glCreateProgram();
	for(const auto& shader : type_fname){
		auto handle = BuildShader(shader.first, file2string(SHADER_ROOT + shader.second));
		glAttachShader(program, handle);
	}
	glLinkProgram(program);
	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
		throw std::runtime_error("Shader could not be linked.");
	}
	return program;
}
template<typename T>
std::vector<T> string2vector(const std::string& str)
{
	std::istringstream stream(str);
	std::vector<T> vec;
	T d;
	while(stream>>d) vec.push_back(d);
	return vec;
}
std::ostream& operator<<(std::ostream& stream, const glm::vec3& v){
	stream << v.x << ',' << v.y << ',' << v.z;
	return stream;
}
// only process v and f
// and f only support 3 point and only vert index
void simpleReadObjFile(const std::string& fname, std::vector<float> &vert,
					   std::vector<float>& normal, std::vector<int> &index)
{
	std::string fullname = DATA_ROOT + fname;
	std::cout << "read vert:" << fullname << std::endl;
	std::ifstream file(fullname.c_str());
	assert(file);
	std::string line;
	glm::vec3 maxVert{FLT_MIN,FLT_MIN,FLT_MIN},minVert{FLT_MAX,FLT_MAX,FLT_MAX};
	while(std::getline(file, line)){
		if(line.empty()) continue;
		const char hint = line[0];
		if(hint == 'v'){
			if(line.at(1) == 'n'){
				auto n3 = string2vector<float>(line.substr(2));
				assert(n3.size() == 3);
				normal.insert(normal.end(), n3.begin(), n3.end());
			}
			else{
				auto v3 = string2vector<float>(line.substr(1));
				assert(v3.size() == 3);
				vert.insert(vert.end(), v3.begin(), v3.end());
				glm::vec3 v{v3[0],v3[1],v3[2]};
				maxVert = glm::max(v, maxVert);
				minVert = glm::min(v, minVert);
			}
			
		}
		else if(hint == 'f'){
			auto i3 = string2vector<float>(line.substr(1));
			assert(i3.size() == 3);
			for(int i = 0; i < 3; ++i){
				i3[i]-=1;
			} 
			index.insert(index.end(), i3.begin(), i3.end());
		}
		
	}
	assert(vert.size() == normal.size());
	//normalize the vertices
	glm::vec3 sub = maxVert - minVert;
	//std::cout << maxVert << ' ' << minVert << ' ' << sub << std::endl;
	float scale = *std::max_element(&sub[0], &sub[0] + 3);
	if(scale > 1.0f){
		for(auto& v : vert) v/=scale;
	}
	
}
void APIENTRY DebugFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
						const GLchar* message, const GLvoid* userParam)
{
	std::string srcName;
	switch(source)
	{
	case GL_DEBUG_SOURCE_API_ARB: srcName = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: srcName = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: srcName = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: srcName = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB: srcName = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER_ARB: srcName = "Other"; break;
	}

	std::string errorType;
	switch(type)
	{
	case GL_DEBUG_TYPE_ERROR_ARB: errorType = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: errorType = "Deprecated Functionality"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: errorType = "Undefined Behavior"; break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB: errorType = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB: errorType = "Performance"; break;
	case GL_DEBUG_TYPE_OTHER_ARB: errorType = "Other"; break;
	}

	std::string typeSeverity;
	switch(severity)
	{
	case GL_DEBUG_SEVERITY_HIGH_ARB: typeSeverity = "High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB: typeSeverity = "Medium"; break;
	case GL_DEBUG_SEVERITY_LOW_ARB: typeSeverity = "Low"; break;
	}

	printf("%s from %s,\t%s priority\nMessage: %s\n",
		errorType.c_str(), srcName.c_str(), typeSeverity.c_str(), message);
}
