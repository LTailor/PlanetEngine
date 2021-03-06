#include <iostream>
#include <sstream>
#define GLEW_STATIC

#include <GLEW\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Sphere.h"
#include "Camera.h"
#include "ShaderHelper.h"

const GLchar* vertexShaderSrc =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"out vec4 ppp;"
"uniform mat4 mvp;"
"void main()"
"{"
"   vec4 pos = vec4(pos.x, pos.y, pos.z, 1.0);"
"	ppp=pos;"
"	gl_Position = mvp*pos;"
"}";
const GLchar* fragmentShaderSrc =
"#version 330 core\n"
"in vec4 ppp;"
"out vec4 frag_color;"
"void main()"
"{"
"	float len=length(ppp-vec4(0.0,0.0,0.0,1.0));"
"   frag_color = vec4(0.1,(len-6000)/500.0,0.1,1.0);"
"}";

void showFPS(GLFWwindow* window);
GLFWwindow* init(int windowWidth, int windotHeight, const char* windowTitle);
Camera camera(glm::vec3(0, 0, -100000), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
int main()
{
	GLFWwindow* window = init(1024, 760, "Space ship");
	
	GLuint vbo,vbo2;

	Sphere sphere(6000);

	int vertexCount = 0;

	auto verts = sphere.getVertexData(&vertexCount);
	GLfloat* data = new GLfloat[vertexCount * 3];
	for (int i = 0; i < vertexCount; i++)
	{
		data[i * 3] = verts[i].x;
		data[i * 3 + 1] = verts[i].y;
		data[i * 3 + 2] = verts[i].z;
	}

	int nCount = 0;
	auto norms = sphere.getNormalData(&nCount);
	GLfloat* dataNorm = new GLfloat[nCount * 3];
	for (int i = 0; i < nCount; i++)
	{
		dataNorm[i * 3] = norms[i].x;
		dataNorm[i * 3 + 1] = norms[i].y;
		dataNorm[i * 3 + 2] = norms[i].z;
	}


	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount*3*sizeof(GLfloat), data, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), dataNorm, GL_DYNAMIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	GLuint ibo;

/*	GLuint indices[] = {
		0, 1, 2, 
		0, 2, 3  
	};

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	*/
	GLint result;

	//GLuint program = glCreateProgram();
	GLuint program = ShaderHelper::createShaderProgram("planet");


	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (!result)
	{
		std::cout << "Error! Shader program linker failure. "  << std::endl;
	}

	
	glm::vec3 pos =glm::vec3(0, 0, 0);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 500000.f);
	glm::mat4 cam = camera.getMatrix();
	//glm::mat4 rot = glm::rotate(glm::mat4(1.0), 2, glm::vec3(0.0, 1.0, 0.0));
	model = proj*cam;
	GLuint mvp = glGetUniformLocation(program, "mvp");
	GLuint camv = glGetUniformLocation(program, "cam");
	
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthRange(0.0f, 1.0f);

		sphere.render(camera.getEye(),camera.getCenter());
		verts = sphere.getVertexData(&vertexCount);
		delete[] data;
		data = new GLfloat[vertexCount * 3];
		for (int i = 0; i < vertexCount; i++)
		{
			data[i * 3] = verts[i].x;
			data[i * 3 + 1] = verts[i].y;
			data[i * 3 + 2] = verts[i].z;
		}
		delete[] verts;

	    norms = sphere.getNormalData(&nCount);
		GLfloat* dataNorm = new GLfloat[vertexCount * 3];
		for (int i = 0; i < vertexCount; i++)
		{
			dataNorm[i * 3] = norms[i].x;
			dataNorm[i * 3 + 1] = norms[i].y;
			dataNorm[i * 3 + 2] = norms[i].z;
		}

		delete[] norms;
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), data, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo2);
		glBufferData(GL_ARRAY_BUFFER, nCount * 3 * sizeof(GLfloat), dataNorm, GL_STATIC_DRAW);

		glUseProgram(program);

		glm::mat4 cam = camera.getMatrix();
		model = proj*cam;
		glUniformMatrix4fv(mvp, 1, GL_FALSE, &model[0][0]);
		
		auto dir = camera.getEye() - camera.getCenter();
		glUniform3fv(camv, 1,&(dir / glm::length(dir))[0]);
		glBindVertexArray(vao);
		
	/*	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
		//cout << vertexCount/3.0 << " ";
		/*glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);*/
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		glBindVertexArray(0);
		glfwSwapBuffers(window);
		showFPS(window);

	}
	return 0;
}

void showFPS(GLFWwindow* window)
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // returns number of seconds since GLFW started, as double float

	elapsedSeconds = currentSeconds - previousSeconds;
	ostringstream outs;
	// Limit text updates to 4 times per second
	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		// The C++ way of setting the window title

		outs.precision(3);	// decimal places
		outs << std::fixed
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		// Reset for next average.
		frameCount = 0;
	}

	frameCount++;
}

void on_keyboard(GLFWwindow* window, int key, int scanCode, int action, int mode)
{
	double dist = glm::length(camera.getEye() - glm::vec3(0, 0, 0));

	if (key == GLFW_KEY_UP)
	{

		if (dist< 6000.001) return;
		if (dist > 8500)
		camera.moveForward(1000);
		else if (dist>6100 )
		{
			//	else if (dist > 6550)
			//	{
			camera.moveForward(50);
			std::cout << "Height " << dist << " ";
			//	}
		}
		else if (dist > 6001)
		{
			camera.moveForward(1);
			std::cout << "Height " << dist << " ";
		}
		else if (dist > 6000.5)
		{
			camera.moveForward(0.1);
			std::cout << "Height " << dist << " ";
		}
	}
	if (key == GLFW_KEY_DOWN)
	{
		glm::vec3 eye = camera.getEye();
		eye.z -= 0.5;
		camera.setEye(eye);
	}
	if (key == GLFW_KEY_Q)
	{
		camera.rotateX(3.14/180);
	}
	if (key == GLFW_KEY_A)
	{
		camera.rotateX(-3.14 / 180);
	}
	if (key == GLFW_KEY_Z)
	{
		camera.rotateY(3.14 / 180);
	}
	if (key == GLFW_KEY_X)
	{
		camera.rotateY(-3.14 / 180);
	}
	if (key == GLFW_KEY_W)
	{
		camera.rotateZ(-3.14 / 180);
	}
	if (key == GLFW_KEY_E)
	{
		camera.rotateZ(3.14 / 180);
	}
}

void on_window_size(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
GLFWwindow* init(int windowWidth, int windowHeight, const char* windowTitle)
{
	if (!glfwInit())
	{
		std::cout << "GLFW initialization FAILED" << std::endl;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
	if (!window)
	{
		std::cout << "Window creation FAILED" << std::endl;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window,on_window_size);
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialization FAILED" << std::endl;
	}

	glfwSetKeyCallback(window, on_keyboard);
	glClearColor(0.5f, 0.23f, 0.33f, 1.0f);
	glViewport(0, 0, windowWidth, windowHeight);

	return window;
}

