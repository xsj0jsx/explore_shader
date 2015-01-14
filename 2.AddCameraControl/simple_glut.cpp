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
#include <glutil/MousePoles.h>
GLuint positionBufferObject;
GLuint program;
GLuint vao;
//the camera is at lookat (0,0,0) with radius 200 in world space
const glm::vec3 targetPos(0.0f,0.0f,0.0f);
const glm::fquat orient = glm::angleAxis(float(glm::radians(90)), glm::vec3(0.0f,1.0f,0.0f));
const float init_radius = 200.0f;
//minRadius, maxRadius, largeRadiusDelta, smallRadiusDelta, largePosOffset, smallPosOffset, rotationScale(degree)
glutil::ViewScale controlData_camera{1, 1000, 10, 1, 10, 1, 1};
glutil::ViewPole camera{glutil::ViewData{targetPos, orient, init_radius, 0}, controlData_camera};
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


void init()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const float vertexPositions[] = {
		0.75f, 0.75f, 0.0f,
		0.75f, -0.75f, 0.0f,
		-0.75f, -0.75f, 0.0f
	};

	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	
	const std::string vertexShader(
		"#version 330 compatibility\n"
		"layout(location = 0) in vec3 position;\n"
		"uniform mat4 uModelViewProjectionMatrix;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = uModelViewProjectionMatrix * vec4(position, 1);\n"
		"}\n"
		);

	const std::string fragmentShader(
		"#version 330\n"
		"out vec4 outputColor;\n"
		"void main()\n"
		"{\n"
		"   outputColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
		"}\n"
		);

	GLuint vertShader = BuildShader(GL_VERTEX_SHADER, vertexShader);
	GLuint fragShader = BuildShader(GL_FRAGMENT_SHADER, fragmentShader);

	program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);	
	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		if(!glext_ARB_debug_output)
		{
			GLint infoLogLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar *strInfoLog = new GLchar[infoLogLength + 1];
			glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
			fprintf(stderr, "Linker failure: %s\n", strInfoLog);
			delete[] strInfoLog;
		}

		throw std::runtime_error("Shader could not be linked.");
	}
}

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void display()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glm::mat4 world2camera = camera.CalcMatrix();
	
	float aspectRadio = float(glutGet(GLUT_WINDOW_WIDTH)) / glutGet(GLUT_WINDOW_HEIGHT);
	glm::mat4 projection = glm::perspective(float(glm::radians(45.0)), aspectRadio, 0.1f, 1000.0f);
	glm::mat4 modelViewProjection = projection * world2camera;
	auto loc_modelViewProjection = glGetUniformLocation(program, "uModelViewProjectionMatrix");
	glUniformMatrix4fv(loc_modelViewProjection, 1, GL_FALSE, glm::value_ptr(modelViewProjection) );
	glDrawArrays(GL_TRIANGLES, 0, 3);
	
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
