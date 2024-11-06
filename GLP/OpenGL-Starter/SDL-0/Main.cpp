#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <SDL.h>
#include <glew.h>

using namespace std;

//Functions
float Distance(float ax, float ay, float bx, float by);
float MoveToward(float origin, float target, float amount);
float Sign(float value);

string LoadShader(string fileName);

//struct
struct Position {
	float x = 0.0f;
	float y = 0.0f;
};

struct SnakeBodyPart 
{
	float posX = 0.0f;
	float posY = 0.0f;

	std::vector<Position> nextPositions;
};

//main
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
	int width = 800;
	int height = 800;

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
	//Make a snake

	std::vector<SnakeBodyPart> snakeBody;
	snakeBody.push_back(SnakeBodyPart{});
	SnakeBodyPart* headBodyPart = &snakeBody[0];

	float appleScale = 0.02f;
	float snakeScale = 0.05f;

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
	unsigned int vbo;
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
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	///////////// MAIN RUN LOOP /////////////
	//variables
	float deltaTime = 0.0f;
	float lastTime = 0.0f;

	//snake
	float snakeSpeedX = 1.0f;
	float snakeSpeedY = 0.0f;
	bool needNewSnakeBodyPart = false;

	//apple
	float appleOffsetX = 0.0f;
	float appleOffsetY = 0.0f;
	bool needNewApple = true;

	bool isTurning = false;
	bool isRunning = true;

	Position previousDirection{};

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

				if (event.key.keysym.sym == SDLK_g) {
					needNewSnakeBodyPart = true;
				}

				//snake movement
				previousDirection = { snakeSpeedX, snakeSpeedY };

				if (event.key.keysym.sym == SDLK_z) {
					snakeSpeedY = 1.0f;
					snakeSpeedX = 0.0f;
					isTurning = true;
				}
				else if (event.key.keysym.sym == SDLK_d) {
					snakeSpeedY = 0.0f;
					snakeSpeedX = 1.0f;
					isTurning = true;
				}
				else if (event.key.keysym.sym == SDLK_q) {
					snakeSpeedY = 0.0f;
					snakeSpeedX = -1.0f;
					isTurning = true;
				}
				else if (event.key.keysym.sym == SDLK_s) {
					snakeSpeedY = -1.0f;
					snakeSpeedX = 0.0f;
					isTurning = true;
				}

				if(isTurning && ( previousDirection.x != snakeSpeedX || previousDirection.y != snakeSpeedY ) && snakeBody.size() > 1)
				{
					float headPosX = headBodyPart->posX;
					float headPosy = headBodyPart->posY;

					for(int i = 1; i < snakeBody.size(); i++)
					{
						snakeBody[i].nextPositions.push_back(Position{ headPosX, headPosy });
					}
					isTurning = false;
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

		//new pos of apple
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
		headBodyPart->posX += deltaTime * snakeSpeedX / 2.0f;
		headBodyPart->posY += deltaTime * snakeSpeedY / 2.0f;

		//collision between snake and apple
		if (needNewSnakeBodyPart || abs(headBodyPart->posX - appleOffsetX) < appleScale + snakeScale && abs(headBodyPart->posY - appleOffsetY) < appleScale + snakeScale) {
			needNewApple = true;
			needNewSnakeBodyPart = false;

			SnakeBodyPart& lastBody = snakeBody.back();

			Position backward{ -snakeSpeedX, -snakeSpeedY };
			if (snakeBody.size() > 1)
			{
				backward.x = Sign(lastBody.posX - (snakeBody.end() - 2)->posX);
				backward.y = Sign(lastBody.posY - (snakeBody.end() - 2)->posY);
				printf("%f %f\n", backward.x, backward.y);
			}

			SnakeBodyPart bodyPart{};
			bodyPart.posX = lastBody.posX + backward.x * snakeScale * 2;
			bodyPart.posY = lastBody.posY + backward.y * snakeScale * 2;

			snakeBody.push_back(bodyPart);
			headBodyPart = &snakeBody[0]; //reassign because of possible vector memory relocation
		}

		//snake death
		if (headBodyPart->posX >= (1 - snakeScale) || headBodyPart->posX <= -1 * (1 - snakeScale)) {
			printf("YOU DIED\n");
		}
		if (headBodyPart->posY >= (1 - snakeScale) || headBodyPart->posY <= -1 * (1 - snakeScale)) {
			printf("YOU DIED\n");
		}

		int offsetLocationX = glGetUniformLocation(snakeShaderProgram, "offsetX");
		int offsetLocationY = glGetUniformLocation(snakeShaderProgram, "offsetY");

		glUseProgram(snakeShaderProgram);

		glUniform1f(offsetLocationX, headBodyPart->posX);
		glUniform1f(offsetLocationY, headBodyPart->posY);

		glDrawArrays(GL_TRIANGLE_FAN, 4, 8);

		const float movementStep = deltaTime / 2.0f;

		for (int i = 1; i < snakeBody.size(); i++) {
			SnakeBodyPart &bodyPart = snakeBody[i];

			if (bodyPart.nextPositions.size() > 0) {

				Position firstPos = bodyPart.nextPositions[0];

				bodyPart.posX = MoveToward(bodyPart.posX, firstPos.x, movementStep);
				bodyPart.posY = MoveToward(bodyPart.posY, firstPos.y, movementStep);

				if (bodyPart.posX == firstPos.x && bodyPart.posY == firstPos.y)
				{
					bodyPart.nextPositions.erase(bodyPart.nextPositions.begin());
				}
			}
			else
			{
				bodyPart.posX = MoveToward(bodyPart.posX, headBodyPart->posX - snakeSpeedX * snakeScale * 2, movementStep);
				bodyPart.posY = MoveToward(bodyPart.posY, headBodyPart->posY - snakeSpeedY * snakeScale * 2, movementStep);

				//SnakeBodyPart& previousBody = snakeBody[i-1];

				//Position backward{ -snakeSpeedX, -snakeSpeedY };

				//if(snakeBody.size() > 1)
				//{
				//	backward.x = Sign(previousBody.posX - snakeBody[i-1].posX);
				//	backward.y = Sign(previousBody.posY - snakeBody[i-1].posY);
				//	printf("%f %f\n", backward.x, backward.y);
				//}

				////SnakeBodyPart bodyPart{};
				//bodyPart.posX = previousBody.posX + backward.x * snakeScale * 2;
				//bodyPart.posY = previousBody.posY + backward.y * snakeScale * 2;
			}

			//render body part
			glUniform1f(offsetLocationX, bodyPart.posX);
			glUniform1f(offsetLocationY, bodyPart.posY);
			glDrawArrays(GL_TRIANGLE_FAN, 4, 8);

		}

		glBindVertexArray(vao);

		SDL_GL_SwapWindow(Window);
	}

	// Quit
	SDL_DestroyWindow(Window);
	SDL_GL_DeleteContext(Context);

	return 0;
}

float MoveToward(float origin, float target, float amount)
{
	if(origin == target)
	{
		return origin;
	}
	if(origin < target)
	{
		return min(origin + amount, target);
	}

	return max(origin - amount, target);
}

float Distance(float ax, float ay, float bx, float by) {
	return sqrt(pow(ax - bx, 2) + pow(ay - by, 2));
}

float Sign(float value)
{
	if(value == 0.0f)
	{ 
		return 0.0f;
	};
	return value < 0.0f ? -1.0f : 1.0f;
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
