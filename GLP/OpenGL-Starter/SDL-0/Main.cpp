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
	//Create a simple window
	int width = 800;
	int height = 800;
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
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


	///////////// DATAS /////////////
	//Make a paper boat
	//Describe the shape by its vertices
	float vertices[] = {
	// positions         //colors
		// lower boat         
		-0.4f, 0.0f, 0.0f, 0.412, 0.318, 0.275,
		-0.2f, 0.0f, 0.0f, 0.412, 0.318, 0.275,
		-0.2f, -0.25f, 0.0f, 0.231, 0.18, 0.161,
		0.0f, 0.0f, 0.0f, 0.412, 0.318, 0.275,
		0.0f, -0.25f, 0.0f, 0.231, 0.18, 0.161,
		0.2f, 0.0f, 0.0f, 0.412, 0.318, 0.275,
		0.2f, -0.25f, 0.0f, 0.231, 0.18, 0.161,
		0.4f, 0.0f, 0.0f, 0.412, 0.318, 0.275,
		// upper boat
		-0.2f, 0.0f, 0.0f, 0.769, 0.647, 0.592,
		0.0f, 0.25f, 0.0f, 0.961, 0.933, 0.922,
		0.0f, 0.0f, 0.0f, 0.769, 0.647, 0.592,
		0.2f, 0.0f, 0.0f, 0.769, 0.647, 0.592,

		//water
		-1.0f, -0.15f, 0.0f, 0.357, 0.584, 0.831,
		-1.0f, -1.0f, 0.0f, 0.357, 0.584, 0.831,
		1.0f, -1.0f, 0.0f, 0.357, 0.584, 0.831,
		1.0f, -0.15f, 0.0f, 0.357, 0.584, 0.831
	};


	///////////// VBO /////////////
	//Create an ID to be given at object generation
	unsigned int vbo;
	//Pass how many buffers should be created and the reference of the ID to get the value set
	glGenBuffers(1, &vbo);

	//boat shader
	string vertexFile = LoadShader("simpleVertex.shader");
	const char *vertexShaderSource = vertexFile.c_str();

	string fragmentFile = LoadShader("simpleFragment.shader");
	const char* fragmentShaderSource = fragmentFile.c_str();


	//Vertex Shader
	unsigned int vertexShaderId;
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexShaderSource, nullptr);
	//Compile vertex shader
	glCompileShader(vertexShaderId);

	//Fragment Shader
	unsigned int fragmentShaderId;
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, nullptr);
	//Compile fragment shader
	glCompileShader(fragmentShaderId);

	//Create program to link the vertex and fragment shaders
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	//Attach shaders to use to the program
	glAttachShader(shaderProgram, vertexShaderId);
	glAttachShader(shaderProgram, fragmentShaderId);

	//Link it 
	glLinkProgram(shaderProgram);
	//now that the program is complete, we can use it 
	glUseProgram(shaderProgram);



	//water shader
	vertexFile = LoadShader("staticVertex.shader");
	const char* staticVertexShaderSource = vertexFile.c_str();


	//Vertex Shader
	unsigned int staticVertexId;
	staticVertexId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(staticVertexId, 1, &staticVertexShaderSource, nullptr);
	//Compile vertex shader
	glCompileShader(staticVertexId);


	//Create program to link the vertex and fragment shaders
	unsigned int staticShaderProgram;
	staticShaderProgram = glCreateProgram();

	//Attach shaders to use to the program
	glAttachShader(staticShaderProgram, staticVertexId);
	glAttachShader(staticShaderProgram, fragmentShaderId);

	//Link it 
	glLinkProgram(staticShaderProgram);
	//now that the program is complete, we can use it 
	glUseProgram(staticShaderProgram);


	///////////// VAO /////////////
	//Create one ID to be given at object generation
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Binds the buffer linked to this ID to the vertex array buffer to be rendered. Put 0 instead of vbo to reset the value.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//Finally send the vertices array in the array buffer 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	///////////// MAIN RUN LOOP /////////////

	float deltaTime = 0.0f;
	float lastTime = 0.0f;

	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float speedX = 2.0f;
	float speedY = 2.0f;

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

		glUseProgram(staticShaderProgram);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

		//DRAW
		float timeValue = ((float)SDL_GetTicks() / 1000);
		deltaTime = timeValue - lastTime;
		lastTime = timeValue;

		offsetX += deltaTime * speedX / 2.0f;

		if (offsetX >= 0.6f || offsetX <= -0.6f) {
			speedX = -speedX;
		}

		int offsetLocationX = glGetUniformLocation(shaderProgram, "offsetX");
		int offsetLocationY = glGetUniformLocation(shaderProgram, "offsetY");

		//Shader to use next
		glUseProgram(shaderProgram);

		glUniform1f(offsetLocationX, offsetX);
		glUniform1f(offsetLocationY, offsetY);

		//VAO to use next
		glBindVertexArray(vao);

		//We draw from vertex 0 and we will be drawing 8 vertices
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
		glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);

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
