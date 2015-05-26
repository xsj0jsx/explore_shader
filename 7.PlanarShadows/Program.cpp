#include "Program.h"



#include <glm/gtc/type_ptr.hpp>


typedef std::map<GLuint, std::string> ShaderType_Fname;
GLuint buildProgrameFromFile(const  ShaderType_Fname &type_fname);

Program::Program(const ShaderTypeFileName& typeName){
  handle_ = buildProgrameFromFile(typeName);
}
Program::~Program(){
  glDeleteProgram(handle_);
  for(auto shader : shaderHandle_) glDeleteShader(shader.second);
}
bool Program::isInUse(){
  GLint data=0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &data);
  return data == handle_;
}
void Program::use(){
  glUseProgram(handle_);
}
void Program::disUse(){
  glUseProgram(0);
}
GLuint Program::uniformLocation(const std::string& name){
  LOG_IF(!isInUse(), WARNING) << "try to get uniformLocation without use first";
  auto location = glGetUniformLocation(handle_, name.c_str());
  LOG_IF(location == -1, WARNING) << "query uniformLocation with "<< name << " failed";
  return location;
}
void Program::set(const std::string& name, const glm::mat4& value){
  set(uniformLocation(name), value);
}
void Program::set(GLuint location, const glm::mat4& mat){
  beforeSetCheck();
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}
void Program::set(const std::string& name, const glm::vec3& value){
  set(uniformLocation(name), value);
}
void Program::set(GLuint location, const glm::vec3& value){
  beforeSetCheck();
  glUniform3f(location, value.x, value.y, value.z);
}
