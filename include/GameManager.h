#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

#include <tuple>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <SDL.h>
#include <glm/glm.hpp>

#include "Timer.h"
#include "GLUtils/GLUtils.hpp"
#include "Model.h"
#include "VirtualTrackball.h"
#include "TextureFBO.h"

/**
 * This class handles the game logic and display.
 * Uses SDL as the display manager, and glm for 
 * vector and matrix computations
 */
class GameManager {
public:

	/**
	 * Constructor
	 */
	GameManager();

	/**
	 * Destructor
	 */
	~GameManager();

	/**
	 * Initializes the game, including the OpenGL context
	 * and data required
	 */
	void init();

	/**
	 * The main loop of the game. Runs the SDL main loop
	 */
	void play();

	/**
	 * Quit function
	 */
	void quit();

	/**
	 * Function that handles rendering into the OpenGL context
	 */
	void render();

protected:
	/**
	 * Creates the OpenGL context using SDL
	 */
	void createOpenGLContext();

	/**
	 * Sets states for OpenGL that we want to keep persistent
	 * throughout the game
	 */
	void setOpenGLStates();

	/**
	 * Creates the matrices for the OpenGL transformations,
	 * perspective, etc.
	 */
	void createMatrices();

	/**
	 * Compiles, attaches, links, and sets uniforms for
	 * a simple OpenGL program
	 */
	void createSimpleProgram();

	/**
	 * Creates vertex array objects
	 */
	void createVAO();

	/**
	  * Sets up the FBO for us
	  */
	void createFBO();

	static const unsigned int window_width = 800;
	static const unsigned int window_height = 600;

private:
	static void renderMeshRecursive(MeshPart& mesh, const std::shared_ptr<GLUtils::Program>& program, const glm::mat4& modelview, const glm::mat4& transform);

	static const unsigned int max_vaos = 2;
	GLuint vaos[max_vaos]; //< Vertex array object
	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > vertices;
	std::shared_ptr<GLUtils::BO<GL_ELEMENT_ARRAY_BUFFER> > indices;
	std::shared_ptr<GLUtils::Program> phong_program, bloom_program, horizontal_blur_program, vertical_blur_program;

	std::shared_ptr<Model> model;
	//Fbos for rendering
	std::shared_ptr<TextureFBO> fbo1;
	std::shared_ptr<TextureFBO> fbo2;

	Timer my_timer; //< Timer for machine independent motion

	glm::mat4 projection_matrix; //< OpenGL projection matrix
	glm::mat4 model_matrix; //< OpenGL model transformation matrix
	glm::mat4 view_matrix; //< OpenGL camera/view matrix
	glm::mat3 normal_matrix; //< OpenGL matrix to transfor normals
	glm::mat4 trackball_view_matrix; //< OpenGL camera matrix for the trackball
	
	SDL_Window* main_window; //< Our window handle
	SDL_GLContext main_context; //< Our opengl context handle 
	
	VirtualTrackball trackball;

	static GLubyte quad_indices[];
	static GLfloat quad_vertices[];
	static unsigned int downscale_level;
};

#endif // _GAMEMANAGER_H_
