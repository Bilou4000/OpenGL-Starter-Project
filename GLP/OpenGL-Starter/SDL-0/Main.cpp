#include <iostream>
#include <SDL.h>
#include <glew.h>

using namespace std;
//#define GLEW_STATIC

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		cout << "SDL initialization failed. SDL Error: " << SDL_GetError();
	}
	else
	{
		cout << "SDL initialization succeeded!\n";
	}

	///////////SETTING UP SDL/////////////
	//Create a simple window
	int width = 400;
	int height = 300;
	unsigned int center = SDL_WINDOWPOS_CENTERED;
	SDL_Window* Window = SDL_CreateWindow("My window", center, center, width, height, SDL_WINDOW_OPENGL);
	//SDL_WINDOW_OPENGL is a u32 flag !


	//Create an OpenGL compatible context to let glew draw on it
	SDL_GLContext Context = SDL_GL_CreateContext(Window);

	/////////SETTING UP OPENGL WITH GLEW///
	//Initialize glew
	glewExperimental = GL_TRUE;
	if (glewInit() == GLEW_OK) {
		cout << "Glew initialized successfully\n";
	}

	//Set the viewing frame through which we will see the objects
	glViewport(0, 0, width, height);

	//Put the color you want here for the background
	glClearColor(0.7f, 0.4f, 0.3f, 1.0f);


	//Make a triangle
	//Describe the shape by its vertices
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	//Create an ID to be given at object generation
	unsigned int vbo;

	//Pass how many buffers should be created and the reference of the ID to get the value set
	glGenBuffers(1, &vbo);


	bool isRunning = true;

	while (isRunning) {
		// Inputs
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				isRunning = false;
				break;
			default:
				break;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen
		SDL_GL_SwapWindow(Window); // Swapbuffer
	}

	// Quit
	SDL_DestroyWindow(Window);
	SDL_GL_DeleteContext(Context);

	return 0;
}