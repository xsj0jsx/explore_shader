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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp>
#include <glutil/MousePoles.h>
#include <vector>
#include <map>
void APIENTRY DebugFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
						const GLchar* message, const GLvoid* userParam);
typedef std::map<GLuint, std::string> ShaderType_Fname;
GLuint buildProgrameFromFile(const  ShaderType_Fname &type_fname);
void simpleReadObjFile(const std::string& fname, std::vector<float> &vert,
					   std::vector<float> &normal, std::vector<int> &index);

GLuint programe_pointADSLight, programe_spotADSLight, programe_flatShadeLight, programe_phongPoint,
	programe_halfAnglePhong;
GLuint programe_current;

GLuint vao;
int draw_vert_number;
//the camera is at lookat (0,0,0) with radius 100 in world space
const glm::vec3 targetPos(0.0f,0.0f,0.0f);
const glm::fquat orient = glm::angleAxis(float((0)), glm::vec3(0.0f,1.0f,0.0f));
const float init_radius = 100.0f;
//minRadius, maxRadius, largeRadiusDelta, smallRadiusDelta, largePosOffset, smallPosOffset, rotationScale(degree)
glutil::ViewScale controlData_camera{1, 1000, 10, 5, 10, 1, 1};
glutil::ViewPole camera{glutil::ViewData{targetPos, orient, init_radius, 0}, controlData_camera};



void init()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	std::vector<float> vert, normal, vn_buffer;
	std::vector<int> index;
	
	simpleReadObjFile("teapot.obj", vert, normal, index);
	draw_vert_number = index.size();
	GLuint positionBufferObject;
	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	vn_buffer.insert(vn_buffer.end(), vert.begin(), vert.end());
	vn_buffer.insert(vn_buffer.end(), normal.begin(), normal.end());
	glBufferData(GL_ARRAY_BUFFER, vn_buffer.size() * sizeof(float), &vn_buffer[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(vert.size() * sizeof(float)));
	GLuint elementBufferObject;
	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(int), &index[0], GL_STATIC_DRAW);
	
	ShaderType_Fname spotADSLight_shaders = {{GL_VERTEX_SHADER, "spot_vertex.shader"}
											 ,{GL_FRAGMENT_SHADER, "fragment.shader"} };
	programe_spotADSLight = buildProgrameFromFile(spotADSLight_shaders);
	ShaderType_Fname flatPoint_shaders = {{GL_VERTEX_SHADER, "flat_vertex.shader"}
										  ,{GL_FRAGMENT_SHADER, "flat_fragment.shader"} };
	programe_flatShadeLight = buildProgrameFromFile(flatPoint_shaders);
	ShaderType_Fname phongPoint_shaders = { {GL_VERTEX_SHADER, "phong_vertex.shader"}
											,{GL_FRAGMENT_SHADER, "phong_fragment.shader"} };
	programe_phongPoint = buildProgrameFromFile(phongPoint_shaders);
	ShaderType_Fname halfAngle_shaders = { {GL_VERTEX_SHADER, "phong_vertex.shader"}
										   ,{GL_FRAGMENT_SHADER, "halfAngle_fragment.shader"}};
	programe_halfAnglePhong = buildProgrameFromFile(halfAngle_shaders);
	//
	ShaderType_Fname adsLight_shaders = {{GL_VERTEX_SHADER, "ads_vertex.shader"}
										  ,{GL_FRAGMENT_SHADER, "fragment.shader"}};
	programe_pointADSLight = buildProgrameFromFile(adsLight_shaders);
	//
	programe_current = programe_pointADSLight;
}

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void display()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//std::vector<GLuint> the_programe{programe_directionLight, show_normal_program};
	glm::mat4 obj2world = glm::scale(1.0f,1.0f,1.0f);
	glm::mat4 world2camera = camera.CalcMatrix() * obj2world;
	float aspectRadio = float(glutGet(GLUT_WINDOW_WIDTH)) / glutGet(GLUT_WINDOW_HEIGHT);
	glm::mat4 projection = glm::perspective(float(glm::radians(45.0)), aspectRadio, 0.1f, 1000.0f);
	glm::mat4 modelViewProjection = projection * world2camera;
	glm::mat4 normal2view = glm::inverseTranspose(world2camera);
	{
		auto program = programe_current;
		glUseProgram(program);
		auto loc_model2view = glGetUniformLocation(program, "uModelViewMatrix");
		glUniformMatrix4fv(loc_model2view, 1, GL_FALSE, glm::value_ptr(world2camera));
		auto loc_modelViewProjection = glGetUniformLocation(program, "uModelViewProjectionMatrix");
		glUniformMatrix4fv(loc_modelViewProjection, 1, GL_FALSE, glm::value_ptr(modelViewProjection) );
		auto loc_normalView = glGetUniformLocation(program, "uNormalViewMatrix");
		glUniformMatrix4fv(loc_normalView, 1, GL_FALSE, glm::value_ptr(normal2view) );
		glDrawElements(GL_TRIANGLES, draw_vert_number, GL_UNSIGNED_INT, 0);
	}
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
	case '1':
		programe_current = programe_pointADSLight;
		break;
	case '2':
		programe_current = programe_spotADSLight;
		break;
	case '3':
		programe_current = programe_flatShadeLight;
		break;
	case '4':
		programe_current = programe_phongPoint;
		break;
	case '5':
		programe_current = programe_halfAnglePhong;
		break;
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
