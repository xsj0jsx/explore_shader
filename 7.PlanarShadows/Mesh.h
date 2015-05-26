#ifndef MESH_HEADER
#define MESH_HEADER
#include<memory>
#include<vector>
#include<string>
#include<glm/glm.hpp>
#include <glload/gl_3_3.h>
class Mesh{
  GLuint vao, dataBuffer, elementBuffer;
  GLenum primitiveType_;
  unsigned int vertexNumber_;
  Mesh(const Mesh&);
  const Mesh& operator=(const Mesh&);
public:
  static const int locPosition = 0;
  static const int locNormal = 1;
  static const int NumberComponetPosition = 3, NumberComponetNormal = 3;
  typedef std::vector<glm::vec3> ArrayVert;
  typedef std::vector<glm::vec3> ArrayNormal;
  typedef std::vector<int> ArrayIndex;
  Mesh(ArrayVert& vert, ArrayNormal& normal, ArrayIndex& index, GLenum primitiveType);
  ~Mesh();
  void draw();
};
typedef std::shared_ptr<Mesh> MeshPtr;
#endif
