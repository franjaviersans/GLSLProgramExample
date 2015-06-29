#include "Definitions.h"
#include "GLSLProgram.h"
#include "FBOCube.h"
#include <iostream>
#include <stdexcept>


using std::cout;
using std::cerr;
using std::endl;



namespace glfwFunc
{
	GLFWwindow* glfwWindow;
	int WINDOW_WIDTH = 1024;
	int WINDOW_HEIGHT = 768;
	std::string strNameWindow = "Hello GLFW";
	GLuint m_texture;

	const float NCP = 0.01f;
	const float FCP = 45.0f;
	const float fAngle = 45.f;

	bool pintar = false;

	GLSLProgram * m_program, * m_computeProgram, * m_computeVisualizationProgram;
	glm::mat4x4 mProjMatrix, mModelViewMatrix;

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


		//First Cube
		mModelViewMatrix =  glm::translate(glm::mat4(), glm::vec3(-1.0f,0.0f,-3.0f)) * 
							RotationMat; 


		//Draw a Cube
		m_program->use();
		{
			m_program->setUniform("mProjection", mProjMatrix);
			m_program->setUniform("mModelView", mModelViewMatrix);
			FBOCube::Instance()->DrawPatch();
		}
	

		//Second Cube with texture
		mModelViewMatrix =  glm::translate(glm::mat4(), glm::vec3(1.0f,0.0f,-3.0f)) * 
							RotationMat; 

		//Draw a Cube
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		m_computeVisualizationProgram->use();
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texture);
			m_computeVisualizationProgram->setUniform("mProjection", mProjMatrix);
			m_computeVisualizationProgram->setUniform("mModelView", mModelViewMatrix);
			m_computeVisualizationProgram->setUniform("mtex", 0);
			FBOCube::Instance()->Draw();
		}



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
		m_program = new GLSLProgram();
		m_computeProgram = new GLSLProgram();
		m_computeVisualizationProgram = new GLSLProgram(); 

		try {
			//Program with all the shaders
			m_program->compileShader("./shaders/basic.vert", GLSLShader::VERTEX);
			m_program->compileShader("./shaders/basic.tcs", GLSLShader::TESS_CONTROL);
			m_program->compileShader("./shaders/basic.tes", GLSLShader::TESS_EVALUATION);
			m_program->compileShader("./shaders/basic.geom", GLSLShader::GEOMETRY);
			m_program->compileShader("./shaders/basic.frag", GLSLShader::FRAGMENT);

			m_program->link();
			m_program->use();
			{
				m_program->bindAttribLocation(WORLD_COORD_LOCATION, "vVertex");
				m_program->bindAttribLocation(COLOR_COORD_LOCATION, "vColor");
			}

			//Program to display result of compute shader
			m_computeVisualizationProgram->compileShader("./shaders/compute.vert", GLSLShader::VERTEX);
			m_computeVisualizationProgram->compileShader("./shaders/compute.frag", GLSLShader::FRAGMENT);

			m_computeVisualizationProgram->link();
			m_computeVisualizationProgram->use();
			{
				m_computeVisualizationProgram->bindAttribLocation(WORLD_COORD_LOCATION, "vVertex");
				m_computeVisualizationProgram->bindAttribLocation(TEXTURE_COORD_LOCATION, "vTexCoord");
			}

			//Generate empty texture
			glGenTextures(1, &m_texture);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texture);
			
			glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

			GLfloat * tess = new GLfloat[1024 * 512 * 4];

			for(int i=0;i<1024*512*4;++i) tess[i] = 1.0f;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024, 512, 0, GL_RGBA, GL_FLOAT, tess);


			delete tess;

			//Program with compute shader
			m_computeProgram->compileShader("./shaders/compute.cs", GLSLShader::COMPUTE);
			m_computeProgram->link();
			m_computeProgram->use();
			{
				m_computeProgram->setUniform("radius", 200.0f);

				//Bind the texture
				glBindImageTexture(m_computeProgram->getUniformLocation("destTex"), m_texture, 0, false, 0, GL_WRITE_ONLY, GL_RGBA16F);

				//Do calculation with Compute Shader
				glDispatchCompute((1024 + 8)/8, (512 + 8)/8, 1);

				//bind the default texture to the image unit, hopefully freeing ours for editing
				glBindImageTexture(m_computeProgram->getUniformLocation("destTex"), 0, 0, false, 0, GL_WRITE_ONLY, GL_RGBA16F);
			}
			

			


		} catch(GLSLProgramException & e) {
 			cerr << e.what() << endl;
 			exit( EXIT_FAILURE );
		}
		
		//Needed for tessellation
		glPatchParameteri( GL_PATCH_VERTICES, 4);


		return true;
	}


	/// Here all data must be destroyed + glfwTerminate
	void destroy()
	{
		delete m_program;
		delete m_computeProgram;
		delete m_computeVisualizationProgram;
		glfwDestroyWindow(glfwWindow);
		glfwTerminate();
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
