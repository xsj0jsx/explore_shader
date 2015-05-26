#include "Mesh.h"

Mesh::Mesh(ArrayVert& vert, ArrayNormal& normal, ArrayIndex& index, GLenum primitiveType):
  primitiveType_(primitiveType), vertexNumber_(index.size()){
    
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
    
  glGenBuffers(1, &dataBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);
  const size_t sizeVert = vert.size() * sizeof(vert[0]);
  const size_t sizeNormal = normal.size() * sizeof(normal[0]);
  const size_t sizeBuffer = sizeVert + sizeNormal;
  glBufferData(GL_ARRAY_BUFFER, sizeBuffer, 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVert, (GLvoid*)&vert[0]);
  glBufferSubData(GL_ARRAY_BUFFER, sizeVert, sizeNormal, (GLvoid*)&normal[0]);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, NumberComponetPosition, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, NumberComponetNormal, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeVert));
    
  glGenBuffers(1, &elementBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(index[0]), &index[0], GL_STATIC_DRAW);
  //
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
}
Mesh::~Mesh(){
  glDeleteBuffers(1, &dataBuffer);
  glDeleteBuffers(1, &elementBuffer);
  glDeleteVertexArrays(1, &vao);
}
void Mesh::draw(){
  glBindVertexArray(vao);
  glDrawElements(primitiveType_, vertexNumber_, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
