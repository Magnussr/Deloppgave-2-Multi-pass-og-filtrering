#include "GameManager.h"
#include "GameException.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <stdexcept>

#include "GLUtils/GLUtils.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using std::cerr;
using std::endl;
using GLUtils::BO;
using GLUtils::Program;
using GLUtils::readFile;

//Vertices to render a quad
GLfloat GameManager::quad_vertices[] =  {
	-1.f, -1.f,
	 1.f, -1.f,
	 1.f,  1.f,
	-1.f,  1.f,
};

//Indices to create the quad
GLubyte GameManager::quad_indices[] = {
	0, 1, 2, //triangle 1
	2, 3, 0, //triangle 2
};

unsigned int GameManager::downscale_level = 4;

GameManager::GameManager() {
	my_timer.restart();
	
	//Setts the render mode to standar phong shading 
	filterMode = RenderMode::STANDARD;

}

GameManager::~GameManager() {
}

void GameManager::createOpenGLContext() {
	//Set OpenGL major an minor versions
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Set OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Use double buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16); // Use framebuffer with 16 bit depth buffer
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); // Use framebuffer with 8 bit for red
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8); // Use framebuffer with 8 bit for green
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8); // Use framebuffer with 8 bit for blue
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // Use framebuffer with 8 bit for alpha
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	// Initalize video
	main_window = SDL_CreateWindow("Westerdals - PG6200 Example OpenGL Program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!main_window) {
		THROW_EXCEPTION("SDL_CreateWindow failed");
	}

	//Create OpenGL context
	main_context = SDL_GL_CreateContext(main_window);
	trackball.setWindowSize(window_width, window_height);

	// Init glew
	// glewExperimental is required in openGL 3.3 
	// to create forward compatible contexts 
	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		std::stringstream err;
		err << "Error initializing GLEW: " << glewGetErrorString(glewErr);
		THROW_EXCEPTION(err.str());
	}

	// Unfortunately glewInit generates an OpenGL error, but does what it's
	// supposed to (setting function pointers for core functionality).
	// Lets do the ugly thing of swallowing the error....
	glGetError();
}

void GameManager::setOpenGLStates() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	CHECK_GL_ERRORS();
	glClearColor( .0,  .0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GameManager::createMatrices() {
	projection_matrix = glm::perspective(45.0f,
			window_width / (float) window_height, 1.0f, 10.f);
	model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(3));
	view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
}

void GameManager::createSimpleProgram() {
	//Compile shaders, attach to program object, and link
	phong_program.reset(new Program("shaders/phong_os.vert", "shaders/phong_os.frag"));
	passthrough_program.reset(new Program("shaders/passthrough.vert", "shaders/passthrough.frag"));
	horizontal_blur_program.reset(new Program("shaders/passthrough.vert","shaders/horizontal_blur.frag"));
	vertical_blur_program.reset(new Program("shaders/passthrough.vert","shaders/vertical_blur.frag"));
	greyscale_program.reset(new Program("shaders/passthrough.vert","shaders/greyscale.frag"));
	CHECK_GL_ERRORS();

	//Set uniforms for the programs
	phong_program->use();
	glUniformMatrix4fv(phong_program->getUniform("projection_matrix"), 1, 0, glm::value_ptr(projection_matrix));
	phong_program->disuse();
	CHECK_GL_ERRORS();
	
	passthrough_program->use();
	glUniform1i(passthrough_program->getUniform("my_texture"), 0);

	horizontal_blur_program->use();
	glUniform1i(horizontal_blur_program->getUniform("my_texture"), 0);
	glUniform1f(horizontal_blur_program->getUniform("dx"), 1.0f / fbo2->getWidth());
	CHECK_GL_ERRORS();

	vertical_blur_program->use();
	glUniform1i(vertical_blur_program->getUniform("my_texture"), 1);
	glUniform1f(vertical_blur_program->getUniform("dy"), 1.0f / fbo2->getHeight());
	CHECK_GL_ERRORS();
}

void GameManager::createVAO() {
	glGenVertexArrays(max_vaos, vaos);

	//Load a model into vao 0
	glBindVertexArray(vaos[0]);
	CHECK_GL_ERRORS();
	model.reset(new Model("models/bunny.obj", false));
	model->getVertices()->bind();
	phong_program->setAttributePointer("position", 3);
	CHECK_GL_ERRORS();
	model->getNormals()->bind();
	phong_program->setAttributePointer("normal", 3);
	CHECK_GL_ERRORS();
	vertices->unbind(); //Unbinds both vertices and normals

	//Load the quad into vao 1
	glBindVertexArray(vaos[1]);
	vertices.reset(new BO<GL_ARRAY_BUFFER>(quad_vertices, sizeof(quad_vertices)));
	indices.reset(new BO<GL_ELEMENT_ARRAY_BUFFER>(quad_indices, sizeof(quad_indices)));
	vertices->bind();
	passthrough_program->setAttributePointer("position", 2);
	indices->bind();
	CHECK_GL_ERRORS();

	//Unbind and check for errors
	vertices->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);
	CHECK_GL_ERRORS();
}

void GameManager::createFBO() {
	
	//Create two FBO for multipass rendering
	fbo1.reset(new TextureFBO(window_width, window_height));
	fbo1->unbind();

	fbo2.reset(new TextureFBO(window_width >> downscale_level, window_height >> downscale_level));
	fbo2->unbind();
}

void GameManager::init() {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::stringstream err;
		err << "Could not initialize SDL: " << SDL_GetError();
		THROW_EXCEPTION(err.str());
	}
	atexit(SDL_Quit);

	createOpenGLContext();
	setOpenGLStates();
	createFBO();
	createMatrices();
	createSimpleProgram();
	createVAO();
}

void GameManager::renderMeshRecursive(MeshPart& mesh, const std::shared_ptr<Program>& program, 
		const glm::mat4& view_matrix, const glm::mat4& model_matrix) {
	//Create modelview matrix
	glm::mat4 meshpart_model_matrix = model_matrix*mesh.transform;
	glm::mat4 modelview_matrix = view_matrix*meshpart_model_matrix;
	glUniformMatrix4fv(program->getUniform("modelview_matrix"), 1, 0, glm::value_ptr(modelview_matrix));
	
	glm::mat4 modelview_inverse_matrix = glm::inverse(glm::mat4(modelview_matrix));
	glUniformMatrix4fv(program->getUniform("modelview_inverse_matrix"), 1, 0, glm::value_ptr(modelview_inverse_matrix));
	
	if (mesh.count > 0)
		glDrawArrays(GL_TRIANGLES, mesh.first, mesh.count);
	for (unsigned int i=0; i<mesh.children.size(); ++i)
		renderMeshRecursive(mesh.children.at(i), program, view_matrix, meshpart_model_matrix);
}

void GameManager::render() {
	//Clear screen, and set the correct program
	glm::mat4 view_matrix_new = view_matrix*trackball_view_matrix;

	//Set up rendering to first vbo
	fbo1->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, window_width, window_height);

	//Render model to the FBO
	phong_program->use();
	glBindVertexArray(vaos[0]);
	renderMeshRecursive(model->getMesh(), phong_program, view_matrix_new, model_matrix);

	//Unbind the FBO, and check for errors
	fbo1->unbind();
	CHECK_GL_ERRORS();

	//Switch for the different filters 
	switch (filterMode){

	//Renders Greyscale and blur if its combo mode
	case RenderMode::COMBO:
	case RenderMode::GREYSCALE: {

		//Render greyscale filter to fbo1
		fbo1->bind();
		glDepthMask(GL_FALSE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbo1->getTexture());
		glViewport(0, 0, fbo1->getWidth(), fbo1->getHeight());
		greyscale_program->use();
		glBindVertexArray(vaos[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
		glDepthMask(GL_TRUE);
		fbo1->unbind();


	}if(filterMode == RenderMode::GREYSCALE) break; //breaks only if in grayscale mode
	case RenderMode::BLUR: {

		//Generate mipmaps
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbo1->getTexture());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);


		//blur vertically
		fbo2->bind();
		glDepthMask(GL_FALSE);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, fbo1->getTexture());
		glViewport(0, 0, fbo2->getWidth(), fbo2->getHeight());
		vertical_blur_program->use();
		glBindVertexArray(vaos[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
		glDepthMask(GL_TRUE);
		fbo2->unbind();

		//blur horizontally
		fbo1->bind();
		glDepthMask(GL_FALSE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbo2->getTexture());
		glViewport(0, 0, fbo1->getWidth(), fbo1->getHeight());
		horizontal_blur_program->use();
		glBindVertexArray(vaos[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
		glDepthMask(GL_TRUE);
		fbo1->unbind();

	}break;

	case RenderMode::STANDARD:
	default:
	break;
	}

	//Set up rendering to screen
	glDepthMask(GL_FALSE);
	glViewport(0, 0, window_width, window_height);

	//Render quad to screen, textured with the result of the blur operation
	passthrough_program->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo1->getTexture());
	glBindVertexArray(vaos[1]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));

	//Unbind stuff and check for errors
	glBindTexture(GL_TEXTURE_2D, 0);
	glDepthMask(GL_TRUE);
	CHECK_GL_ERRORS();
	glBindVertexArray(0);
	CHECK_GL_ERRORS();
}

void GameManager::play() {
	bool doExit = false;

	//SDL main loop
	while (!doExit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {// poll for pending events
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				trackball.rotateBegin(event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEBUTTONUP:
				trackball.rotateEnd(event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEMOTION:
				trackball_view_matrix = trackball.rotate(event.motion.x, event.motion.y);
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE: //Esc
					doExit = true;
					break;
				case SDLK_q: //Ctrl+q
					if (event.key.keysym.mod & KMOD_CTRL) doExit = true;
					break;
				case SDLK_0: //Render Standar phong shading
				{
					std::cout << "0" << std::endl;
					if (RenderMode::STANDARD == filterMode) break;
					
					fbo2.reset(new TextureFBO(window_width, window_height));
					fbo2->unbind();
				
					filterMode = RenderMode::STANDARD;
				}
				break;
				case SDLK_1: //Render Blur filtermode
				{
					std::cout << "1" << std::endl;
					if (RenderMode::BLUR == filterMode) break;
				
					fbo2.reset(new TextureFBO(window_width >> downscale_level, window_height >> downscale_level));
					fbo2->unbind();
				
					filterMode = RenderMode::BLUR;
				}
					break;
				case SDLK_2: //Render Greyscale filtermode
				{
					std::cout << "2" << std::endl;
					if (RenderMode::GREYSCALE == filterMode) break;


					fbo2.reset(new TextureFBO(window_width, window_height));
					fbo2->unbind();


					filterMode = RenderMode::GREYSCALE;
				}
					break;
				case SDLK_3: //Render Greyscale and Blur
				{
					std::cout << "3" << std::endl; 
					if (RenderMode::COMBO == filterMode) break;

					fbo2.reset(new TextureFBO(window_width >> downscale_level, window_height >> downscale_level));
					fbo2->unbind();

					filterMode = RenderMode::COMBO;
				}
					break;
				}
				break;
			case SDL_QUIT: //e.g., user clicks the upper right x
				doExit = true;
				break;
			}
		}

		//Render, and swap front and back buffers
		render();
		SDL_GL_SwapWindow(main_window);
	}
	quit();
}

void GameManager::quit() {
	std::cout << "Bye bye..." << std::endl;
}
