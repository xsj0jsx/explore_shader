#include <string>
#include <exception>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>

#include <glload/gl_3_3.h>
#include <glload/gl_load.hpp>
#include <GL/freeglut.h>
#include <glutil/MatrixStack.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp>
#include <glutil/MousePoles.h>
#include <vector>
#include <map>
#include <memory>
#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP


#include "Mesh.h"
#include "Program.h"
class Plane{
  glm::vec3 x0_, n_;
public:
  Plane(const glm::vec3& x0, const glm::vec3& n):x0_(x0),n_(n){}
  glm::mat4 project2thisMatrix(glm::vec3 lightPos){
    using glm::vec4;
    using glm::mat4;
    using glm::vec3;
    vec3& l = lightPos;
    float d = - glm::dot(x0_ , n_);
    LOG(INFO) << "the d " << d;
    vec3& n = n_;
    vec4 c0( glm::dot(n,l) + d - n.x * l.x, - (n.x * l.y), -n.x * l.z, -n.x);
    vec4 c1(-n.y * l.x, glm::dot(n,l) + d - n.y * l.y, -n.y*l.z, -n.y);
    vec4 c2(-n.z*l.x, -n.z*l.y, glm::dot(n,l)+d-n.z*l.z, -n.z);
    vec4 c3(-d * l.x, -d*l.y, -d*l.z, glm::dot(n,l));
    mat4 mat(c0,c1,c2,c3);
    return mat;
  }
};

void APIENTRY DebugFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
						const GLchar* message, const GLvoid* userParam);


void simpleReadObjFile(const std::string& fname, std::vector<float> &vert,
					   std::vector<float> &normal, std::vector<int> &index);

MeshPtr gMeshPtr;
ProgramPtr gProgramPtr, gProgramShadow;

//the camera is at lookat (0,0,0) with radius 100 in world space
const glm::vec3 targetPos(0.0f,0.0f,0.0f);
const glm::fquat orient = glm::angleAxis(float((0)), glm::vec3(0.0f,1.0f,0.0f));
const float init_radius = 100.0f;
//minRadius, maxRadius, largeRadiusDelta, smallRadiusDelta, largePosOffset, smallPosOffset, rotationScale(degree)
glutil::ViewScale controlData_camera{1, 1000, 10, 5, 10, 1, 1};
glutil::ViewPole camera{glutil::ViewData{targetPos, orient, init_radius, 0}, controlData_camera};



void init()
{
  LOG(INFO) << "start init";
	std::vector<float> vert, normal, vn_buffer;
	std::vector<int> index;
	
	simpleReadObjFile("teapot.obj", vert, normal, index);
	Mesh::ArrayVert arrayVert;
	Mesh::ArrayNormal arrayNormal;
	auto nVert = vert.size()/3;
	arrayVert.reserve(nVert);
	arrayNormal.reserve(nVert);
	for(int i = 0; i < nVert; ++i){
	  auto pVert = &vert[3*i];
	  arrayVert.push_back(Mesh::ArrayVert::value_type{pVert[0],pVert[1],pVert[2] });
	  auto pNorm = &normal[3*i];
	  arrayNormal.push_back(Mesh::ArrayNormal::value_type{pNorm[0], pNorm[1], pNorm[2]});
	}
	gMeshPtr.reset(new Mesh(arrayVert, arrayNormal, index, GL_TRIANGLES));
	gProgramPtr.reset(new Program(
				      Program::ShaderTypeFileName{
					{GL_VERTEX_SHADER, "ads_vertex.shader"}
					,{GL_FRAGMENT_SHADER, "fragment.shader"}
				      }
				      )
			  );
	gProgramShadow.reset(new Program(
					 Program::ShaderTypeFileName{
					   {GL_VERTEX_SHADER, "shadow.vertex"},
					     {GL_FRAGMENT_SHADER, "fragment.shader"}
					 }
	));
	using glm::vec3;
	using glm::vec4;
	using glm::mat4;
	
	
	vec3 lightPos(20,20,0);
	Plane plane(vec3(0,-0.5,0), vec3(0,1,0));
	gProgramPtr->use();
	gProgramPtr->set("lightPosition", lightPos);
	gProgramShadow->use();
	auto mat = plane.project2thisMatrix(lightPos);
	gProgramShadow->set("uShadowMatrix", mat);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void display()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gProgramPtr->use();
	glm::mat4 obj2world = glm::scale(1.0f,1.0f,1.0f);
	glm::mat4 world2camera = camera.CalcMatrix() * obj2world;
	float aspectRadio = float(glutGet(GLUT_WINDOW_WIDTH)) / glutGet(GLUT_WINDOW_HEIGHT);
	glm::mat4 projection = glm::perspective(float(glm::radians(45.0)), aspectRadio, 0.1f, 1000.0f);
	glm::mat4 modelViewProjection = projection * world2camera;
	glm::mat4 normal2view = glm::inverseTranspose(world2camera);
	
	  gProgramPtr->set("uModelViewMatrix", world2camera);
	  gProgramPtr->set("uModelViewProjectionMatrix", modelViewProjection);
	  gProgramPtr->set("uNormalViewMatrix", normal2view);
	  
	  gMeshPtr->draw();
	
	gProgramPtr->disUse();
	gProgramShadow->use();
	
	gProgramShadow->set("uModelViewProjectionMatrix", modelViewProjection);
	
	
	gMeshPtr->draw();
	gProgramShadow->disUse();
	glutSwapBuffers();
	glutPostRedisplay();
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape (int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

//Called whenever a key on the keyboard was pressed.
//The key is given by the ''key'' parameter, which is in ASCII.
//It's often a good idea to have the escape key (ASCII value 27) call glutLeaveMainLoop() to 
//exit the program.
void keyboard(unsigned char key, int x, int y)
{
	printf("%c\n", key);
	//camera.CharPress(key);
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	}
}
void mouse(int button, int state, int x,int y)
{
	glutil::MouseButtons theButton = glutil::MB_LEFT_BTN;
	//printf("button %d %s\n", button, state == GLUT_UP?"Up":"Down");
	if(button == 3 || button == 4){
		if(state == GLUT_UP) return;
		camera.MouseWheel(button == 3?1:-1, 0, glm::ivec2(x,y));
	}
	else{
		switch(button){
		case GLUT_LEFT_BUTTON:theButton = glutil::MB_LEFT_BTN;break;
		case GLUT_MIDDLE_BUTTON:theButton = glutil::MB_MIDDLE_BTN;break;
		case GLUT_RIGHT_BUTTON:theButton = glutil::MB_RIGHT_BTN;break;
		default:return;break;
		}
		camera.MouseClick(theButton, state == GLUT_DOWN, 0, glm::ivec2(x,y));
	}
	
}
void motion(int x, int y)
{
	//printf("motion %d,%d\n",x,y);
	camera.MouseMove(glm::ivec2(x,y));
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	int width = 500;
	int height = 500;
	unsigned int displayMode = GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL;

	glutInitDisplayMode(displayMode);
	glutInitContextVersion (3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitWindowSize (width, height); 
	glutInitWindowPosition (300, 200);
	glutCreateWindow (argv[0]);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	glload::LoadFunctions();
	printf("opengl version %d.%d\n", glload::GetMajorVersion(), glload::GetMinorVersion());
	//glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	glDebugMessageCallbackARB(DebugFunc, (void*)15);

	init();

	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMainLoop();
	return 0;
}
