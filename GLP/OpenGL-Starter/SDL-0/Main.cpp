#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <SDL.h>
#include <glew.h>

using namespace std;
//#define GLEW_STATIC

//Shader
const char* vertexShaderSource = "#version 330 core\n"
"in vec3 pos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(pos, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

//Functions
string LoadShader(string fileName);

int main(int argc, char* argv[])
{
	std::srand(std::time(nullptr));

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
	//Make a snake
	float appleScale = 0.02f;
	float snakeScale = 0.05f;

	//Describe the shape by its vertices
	float vertices[] = {
	// positions         //colors
		//apple
		-appleScale, appleScale, 0.0f,
		-appleScale, -appleScale, 0.0f,
		appleScale, -appleScale, 0.0f,
		appleScale, appleScale, 0.0f,

		//snake
		-snakeScale, snakeScale, 0.0f,
		-snakeScale, -snakeScale, 0.0f,
		snakeScale, -snakeScale, 0.0f,
		snakeScale, snakeScale, 0.0f
	};


	///////////// VBO /////////////
	//Create an ID to be given at object generation
	unsigned int vbo;
	//Pass how many buffers should be created and the reference of the ID to get the value set
	glGenBuffers(1, &vbo);

	//snake shader
	string vertexFile = LoadShader("snakeVertex.shader");
	const char *snakeVertexShaderSource = vertexFile.c_str();

	string fragmentFile = LoadShader("snakeFragment.shader");
	const char* snakeFragmentShaderSource = fragmentFile.c_str();

	//Vertex Shader
	unsigned int snakeShaderId;
	snakeShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(snakeShaderId, 1, &snakeVertexShaderSource, nullptr);
	glCompileShader(snakeShaderId);

	//Fragment Shader
	unsigned int snakeFragmentId;
	snakeFragmentId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(snakeFragmentId, 1, &snakeFragmentShaderSource, nullptr);
	glCompileShader(snakeFragmentId);

	unsigned int snakeShaderProgram;
	snakeShaderProgram = glCreateProgram();

	glAttachShader(snakeShaderProgram, snakeShaderId);
	glAttachShader(snakeShaderProgram, snakeFragmentId);
	glLinkProgram(snakeShaderProgram);
	glUseProgram(snakeShaderProgram);



	//apple shader
	vertexFile = LoadShader("appleVertex.shader");
	const char* appleVertexShaderSource = vertexFile.c_str();

	fragmentFile = LoadShader("appleFragment.shader");
	const char* appleFragmentShaderSource = fragmentFile.c_str();

	//Vertex Shader
	unsigned int appleShaderId;
	appleShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(appleShaderId, 1, &appleVertexShaderSource, nullptr);
	glCompileShader(appleShaderId);

	//Fragment Shader
	unsigned int appleFragmentId;
	appleFragmentId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(appleFragmentId, 1, &appleFragmentShaderSource, nullptr);
	glCompileShader(appleFragmentId);

	unsigned int appleShaderProgram;
	appleShaderProgram = glCreateProgram();

	glAttachShader(appleShaderProgram, appleShaderId);
	glAttachShader(appleShaderProgram, appleFragmentId);
	glLinkProgram(appleShaderProgram);
	glUseProgram(appleShaderProgram);


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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	///////////// MAIN RUN LOOP /////////////

	float deltaTime = 0.0f;
	float lastTime = 0.0f;

	float snakeOffsetX = 0.0f;
	float snakeOffsetY = 0.0f;
	float snakeSpeedX = 1.0f;
	float snakeSpeedY = 0.0f;

	//apple
	float appleOffsetX = 0.0f;
	float appleOffsetY = 0.0f;

	bool needNewApple = true;
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
				//snake movement
				if (event.key.keysym.sym == SDLK_z) {
					snakeSpeedY = 1.0f;
					snakeSpeedX = 0.0f;
				}
				else if (event.key.keysym.sym == SDLK_d) {
					snakeSpeedY = 0.0f;
					snakeSpeedX = 1.0f;
				}
				else if (event.key.keysym.sym == SDLK_q) {
					snakeSpeedY = 0.0f;
					snakeSpeedX = -1.0f;
				}
				else if (event.key.keysym.sym == SDLK_s) {
					snakeSpeedY = -1.0f;
					snakeSpeedX = 0.0f;
				}
				break;
			default:
				break;
			}
		} 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen

		float timeValue = ((float)SDL_GetTicks() / 1000);
		deltaTime = timeValue - lastTime;
		lastTime = timeValue;

		if (needNewApple) {
			appleOffsetX = ((std::rand() % 200) - (100 - appleScale * 100)) / 100;
			appleOffsetY = ((std::rand() % 200) - (100 - appleScale * 100)) / 100;
			needNewApple = false;
		}

		//DRAW
		/////// Apple ///////
		int appleOffsetLocationX = glGetUniformLocation(appleShaderProgram, "offsetX");
		int appleOffsetLocationY = glGetUniformLocation(appleShaderProgram, "offsetY");

		glUseProgram(appleShaderProgram);

		glUniform1f(appleOffsetLocationX, appleOffsetX);
		glUniform1f(appleOffsetLocationY, appleOffsetY);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


		/////// Snake ///////

		//snake move
		snakeOffsetX += deltaTime * snakeSpeedX / 2.0f;
		snakeOffsetY += deltaTime * snakeSpeedY / 2.0f;

		if (abs(snakeOffsetX - appleOffsetX) < appleScale + snakeScale && abs(snakeOffsetY - appleOffsetY) < appleScale + snakeScale) {
			needNewApple = true;
		}


		//snake death
		//if (snakeOffsetX >= 0.95f || snakeOffsetX <= -0.95f) {
		//	//death
		//}
		//if (snakeOffsetY >= 0.95f || snakeOffsetY <= -0.95f) {
		//	//death
		//}

		int offsetLocationX = glGetUniformLocation(snakeShaderProgram, "offsetX");
		int offsetLocationY = glGetUniformLocation(snakeShaderProgram, "offsetY");

		glUseProgram(snakeShaderProgram);

		glUniform1f(offsetLocationX, snakeOffsetX);
		glUniform1f(offsetLocationY, snakeOffsetY);

		glDrawArrays(GL_TRIANGLE_FAN, 4, 8);

		//VAO to use next
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
