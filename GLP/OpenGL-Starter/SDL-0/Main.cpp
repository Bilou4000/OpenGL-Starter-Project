#include <iostream>
#include <fstream>
#include <string>

#include <SDL.h>
#include <glew.h>

using namespace std;
//#define GLEW_STATIC

//Functions
string LoadShader(string fileName);

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
	int width = 1280;
	int height = 720;
	unsigned int center = SDL_WINDOWPOS_CENTERED;
	SDL_Window* Window = SDL_CreateWindow("My window", center, center, width, height, SDL_WINDOW_OPENGL);
	SDL_GLContext Context = SDL_GL_CreateContext(Window);

	/////////SETTING UP OPENGL WITH GLEW///
	glewExperimental = GL_TRUE;
	if (glewInit() == GLEW_OK) {
		cout << "Glew initialized successfully\n";
	}

	glViewport(0, 0, width, height);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	///////////// DATAS /////////////
	float vertices[] = {
		-1.0f, -1.0f, -0.0f,
		1.0f, 1.0f, -0.0f,
		-1.0f, 1.0f, -0.0f,
		1.0f, -1.0f, -0.0f
	};

	unsigned int indices[] =
	{
		0, 1, 2,
		0, 3, 1
	};


	///////////// VBO /////////////
	unsigned int vbo;
	glGenBuffers(1, &vbo);

	//boat shader
	string vertexFile = LoadShader("fractalVertex.shader");
	const char* fractalVertexShaderSource = vertexFile.c_str();

	string fragmentFile = LoadShader("fractalFragment.shader");
	const char* fractalFragmentShaderSource = fragmentFile.c_str();


	//Vertex Shader
	unsigned int fractalShaderId;
	fractalShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(fractalShaderId, 1, &fractalVertexShaderSource, nullptr);
	glCompileShader(fractalShaderId);

	//Fragment Shader
	unsigned int fractalFragmentId;
	fractalFragmentId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fractalFragmentId, 1, &fractalFragmentShaderSource, nullptr);
	glCompileShader(fractalFragmentId);

	unsigned int fractalShaderProgram;
	fractalShaderProgram = glCreateProgram();

	glAttachShader(fractalShaderProgram, fractalShaderId);
	glAttachShader(fractalShaderProgram, fractalFragmentId);
	glLinkProgram(fractalShaderProgram);
	glUseProgram(fractalShaderProgram);


	///////////// VAO /////////////
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	///////////// EBO /////////////
	unsigned int ebo;
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	///////////// MAIN RUN LOOP /////////////
	float time = 0;
	bool isRunning = true;

	while (isRunning) {
		// Inputs
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					isRunning = false;
				}
				break;
			default:
				break;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen

		time += 0.001f;

		//DRAW
		float fractalMovement = glGetUniformLocation(fractalShaderProgram, "time");
		glUseProgram(fractalShaderProgram);

		glUniform1f(fractalMovement, time);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(vao);

		SDL_GL_SwapWindow(Window); // Swapbuffer
	}

	// Quit
	SDL_DestroyWindow(Window);
	SDL_GL_DeleteContext(Context);

	return 0;
}

string LoadShader(string fileName) {
	ifstream myFile;
	myFile.open(fileName);
	if (myFile.fail()) {
		cerr << "Error - failed to open " << fileName << endl;
	}
	string fileText = "";
	string line = "";
	while (getline(myFile, line)) {
		fileText += line + '\n';
	}

	myFile.close();
	return fileText;
}
