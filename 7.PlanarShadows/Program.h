#ifndef PROGRAM_HEADER
#define PROGRAM_HEADER
#include "easylogging++.h"
#include <glload/gl_3_3.h>
#include<string>
#include<map>
#include<glm/glm.hpp>
class Program{
public:
  typedef std::map<GLenum, GLuint> ShaderTypeHandle;
  typedef std::map<GLenum, std::string> ShaderTypeFileName;
private:
  GLuint handle_;
  ShaderTypeHandle shaderHandle_;
  const Program& operator=(const Program&);
  Program(const Program&);
  void beforeSetCheck(){
    LOG_IF(!isInUse(), WARNING) << "try to set without use first";
  }
public:
  class Binder{
    Binder& operator=(const Binder&);
    Binder(const Binder&);
  public:
    Binder(GLuint handle){glUseProgram(handle);}
    ~Binder(){glUseProgram(0);}
  };
  Program(const ShaderTypeFileName& typeName);
  ~Program();
  bool isInUse();
  void use();
  void disUse();
  GLuint uniformLocation(const std::string& name);
  void set(const std::string& name, const glm::mat4& value);
  void set(GLuint location, const glm::mat4& mat);
  void set(const std::string& name, const glm::vec3& value);
  void set(GLuint location, const glm::vec3& value);

};
typedef std::shared_ptr<Program> ProgramPtr;
#endif
