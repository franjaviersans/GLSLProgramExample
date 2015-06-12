#include "Definitions.h"
#include "GLSLProgram.h"
#include "FBOCube.h"
#include <iostream>

using std::cout;
using std::endl;



namespace glfwFunc
{
	GLFWwindow* glfwWindow;
	int WINDOW_WIDTH = 1024;
	int WINDOW_HEIGHT = 768;
	std::string strNameWindow = "Hello GLFW";

	const float NCP = 0.01f;
	const float FCP = 45.0f;
	const float fAngle = 45.f;

	bool pintar = false;

	CGLSLProgram * m_program;
	glm::mat4x4 mProjMatrix, mModelViewMatrix, mMVP;

	//Variables to do rotation
	glm::quat quater, q2;
	glm::mat4x4 RotationMat = glm::mat4x4();
	float angle = 0;
	float *vector=(float*)malloc(sizeof(float)*3);
	double lastx, lasty;
	bool pres = false;

	
	///< Callback function used by GLFW to capture some possible error.
	void errorCB(int error, const char* description)
	{
		printf("%s\n",description );
	}


	inline int TwEventMouseWheelGLFW3(GLFWwindow* window, double xoffset, double yoffset)
	{return true;}
	inline int TwEventCharGLFW3(GLFWwindow* window, int codepoint)
	{return true;}
	inline int TwWindowSizeGLFW3(GLFWwindow* window, int width, int height)
	{return true;}


	///
	/// The keyboard function call back
	/// @param window id of the window that received the event
	/// @param iKey the key pressed or released
	/// @param iScancode the system-specific scancode of the key.
	/// @param iAction can be GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
	/// @param iMods Bit field describing which modifier keys were held down (Shift, Alt, & so on)
	///
	void keyboardCB(GLFWwindow* window, int iKey, int iScancode, int iAction, int iMods)
	{
		if (iAction == GLFW_PRESS)
		{
			switch (iKey)
			{
				case GLFW_KEY_ESCAPE:
				case GLFW_KEY_Q:
					glfwSetWindowShouldClose(window, GL_TRUE);
					break;
				case GLFW_KEY_SPACE:
					break;
			}
		}
	}

	inline int TwEventMousePosGLFW3(GLFWwindow* window, double xpos, double ypos)
	{ 
	
		if(pres){
			//Rotation
			float dx = float(xpos - lastx);
			float dy = float(ypos - lasty);

			if(!(dx == 0 && dy == 0)){
				//Calculate angle and rotation axis
				float angle = sqrtf(dx*dx + dy*dy)/50.0f;
					
				//Acumulate rotation with quaternion multiplication
				q2 = glm::angleAxis(angle, glm::normalize(glm::vec3(dy,dx,0.0f)));
				quater = glm::cross(q2, quater);

				lastx = xpos;
				lasty = ypos;
			}
			return false;
		}
		return true;
	}

	int TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods)
	{ 
		pres = false;

		double x, y;   
		glfwGetCursorPos(window, &x, &y);  
			
		if(button == GLFW_MOUSE_BUTTON_LEFT){
			if(action == GLFW_PRESS){
				lastx = x;
				lasty = y;
				pres = true;
			}

			return true;
		}

		return true;
	}
	
	///< The resizing function
	void resizeCB(GLFWwindow* window, int iWidth, int iHeight)
	{

		WINDOW_WIDTH = iWidth;
		WINDOW_HEIGHT = iHeight;

		if(iHeight == 0) iHeight = 1;
		float ratio = iWidth / float(iHeight);
		glViewport(0, 0, iWidth, iHeight);

		mProjMatrix = glm::perspective(float(fAngle), ratio, 1.0f, 10.0f);
	//	mProjMatrix = glm::ortho(-1.0f,1.0f,-1.0f,1.0f,-1.0f,5.0f);


		TwWindowSizeGLFW3(window, iWidth, iHeight);
		
	}

	///< The main rendering function.
	void draw()
	{
		glClearColor(0.15f, 0.15f, 0.15f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RotationMat = glm::mat4_cast(glm::normalize(quater));

		mModelViewMatrix =  glm::translate(glm::mat4(), glm::vec3(0.0f,0.0f,-2.0f)) * 
							RotationMat; 

		//Draw a Cube
		m_program->enable();
			glUniformMatrix4fv(m_program->getLocation("mProjection"), 1, GL_FALSE, glm::value_ptr(mProjMatrix));
			glUniformMatrix4fv(m_program->getLocation("mModelView"), 1, GL_FALSE, glm::value_ptr(mModelViewMatrix));
			//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			//glPointSize(10);
			FBOCube::Instance()->Draw();
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		m_program->disable();		


		mMVP = mProjMatrix * mModelViewMatrix;

		glfwSwapBuffers(glfwWindow);
	}

	///
	/// Init all data and variables.
	/// @return true if everything is ok, false otherwise
	///
	bool initialize()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		//Init GLEW
		glewExperimental = GL_TRUE;
		if(glewInit() != GLEW_OK) 
		{
			printf("- glew Init failed :(\n");
			return false;
		}
		printf("OpenGL version: %s\n", glGetString(GL_VERSION));
		printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		printf("Vendor: %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));

		// send window size events to AntTweakBar
		glfwSetWindowSizeCallback(glfwWindow, resizeCB);
		glfwSetMouseButtonCallback(glfwWindow, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
		glfwSetCursorPosCallback(glfwWindow, (GLFWcursorposfun)TwEventMousePosGLFW3);
		glfwSetScrollCallback(glfwWindow, (GLFWscrollfun)TwEventMouseWheelGLFW3);
		glfwSetKeyCallback(glfwWindow, (GLFWkeyfun)keyboardCB);
		glfwSetCharCallback(glfwWindow, (GLFWcharfun)TwEventCharGLFW3);


		//load the shaders
		m_program = new CGLSLProgram();
		m_program->loadShader(std::string("./shaders/basic.vert"), CGLSLProgram::VERTEX);
		m_program->loadShader(std::string("./shaders/basic.tcs"), CGLSLProgram::TESSELLATION_CONT);
		m_program->loadShader(std::string("./shaders/basic.tes"), CGLSLProgram::TESSELLATION_EVAL);
		m_program->loadShader(std::string("./shaders/basic.geom"), CGLSLProgram::GEOMETRY);
		m_program->loadShader(std::string("./shaders/basic.frag"), CGLSLProgram::FRAGMENT);

		m_program->create_link();
		m_program->enable();
			m_program->addAttribute("vVertex");
			m_program->addAttribute("vColor");
			m_program->addUniform("mProjection");
			m_program->addUniform("mModelView");
		m_program->disable();

		
		//Needed for tessellation
		glPatchParameteri( GL_PATCH_VERTICES, 4);


		return true;
	}


	/// Here all data must be destroyed + glfwTerminate
	void destroy()
	{
		delete m_program;
		glfwTerminate();
		glfwDestroyWindow(glfwWindow);
	}
}

int main(int argc, char** argv)
{
	glfwSetErrorCallback(glfwFunc::errorCB);
	if (!glfwInit())	exit(EXIT_FAILURE);
	glfwFunc::glfwWindow = glfwCreateWindow(glfwFunc::WINDOW_WIDTH, glfwFunc::WINDOW_HEIGHT, glfwFunc::strNameWindow.c_str(), NULL, NULL);
	if (!glfwFunc::glfwWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(glfwFunc::glfwWindow);
	if(!glfwFunc::initialize()) exit(EXIT_FAILURE);
	glfwFunc::resizeCB(glfwFunc::glfwWindow, glfwFunc::WINDOW_WIDTH, glfwFunc::WINDOW_HEIGHT);	//just the 1st time

	


	// main loop!
	while (!glfwWindowShouldClose(glfwFunc::glfwWindow))
	{
		glfwFunc::draw();
		glfwPollEvents();	//or glfwWaitEvents()
	}

	glfwFunc::destroy();
	return EXIT_SUCCESS;
}
