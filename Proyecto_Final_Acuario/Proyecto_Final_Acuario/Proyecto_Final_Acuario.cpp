#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "modelAnim.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void animacion();
void circuitoTiburon1();
void circuitoTiburon2();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 8.0f, 45.0f));  //Posicion de la camara
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0.0f;
float movCamera = 0.0f;

//Variables para animación del agua

float tiempo = 1.0f;
float speed = 1.0f; //Valor para jugar con la velocidad del movimiento

//Variables para animación de los tiburones

bool recorridoT1 = true;
bool recorridoT2 = true;
bool estado1 = true;
bool estado2 = false;
bool estado3 = false;
bool estado4 = false;
bool estado5 = false;

bool punto1 = true;
bool punto2 = false;
bool punto3 = false;
bool punto4 = false;
bool punto5 = false;


glm::vec3 posInicT1 = glm::vec3(-102.616f, 10.587f, -137.491f);
glm::vec3 posInicT2 = glm::vec3(-6.058f, 4.749f, -82.814f);
glm::vec3 posInicT3 = glm::vec3(-6.058f, 16.534f, -82.814f);


float movTibX = 0.0f;
float movTibZ = 0.0f;
float rotTib = 0.0;

float movTib2X = 0.0f;
float movTib2Z = 0.0f;
float rotTib2 = 0.0;
//Variables para animación de la cola del tiburon

bool izquierda = true;
bool derecha = false;
float rotCola = 0.0f;
//float rotCola2 = 0.0f;


// Variables para la animación de las tortugas

glm::vec3 posInicT1Cuerpo = glm::vec3(-345.848f, 10.098f, -28.859f);//Posicion inicial cuerpo Tortuga 1
glm::vec3 posInicT1AletaTD = glm::vec3(-347.383f, 9.494f, -32.488f);//Posicion inicial aleta trasera derecha
glm::vec3 posInicT1AletaTI = glm::vec3(-344.282f, 9.44f, -32.417f); //Posicion inicial aleta trasera izquierda
glm::vec3 posInicT1AletaDD = glm::vec3(-347.772f, 9.771f, -25.811f);//Posicion inicial aleta delantera derecha
glm::vec3 posInicT1AletaDI = glm::vec3(-343.79f, 9.758f, -25.875f); //Posicion inicial aleta delantera izquierda
//Aleta trase

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(-95.0f, 1.0f, -45.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

bool active;

//Variables y vector para el cambio de luz ambiental
glm::vec3 luzAmbiental = glm::vec3(0.5f, 0.5f, 0.5f);
bool cambioAmbiental = true;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
float posX =PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotRodIzq = 0;

#define MAX_FRAMES 9
int i_max_steps = 190;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ
	float rotRodIzq;
	float rotInc;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(posX,posY,posZ),
	glm::vec3(0,0,0),
	glm::vec3(0,0,0),
	glm::vec3(0,0,0)
};

glm::vec3 LightP1;




void saveFrame(void)
{

	printf("posx %f\n", posX);
	
	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;
	
	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;

}

void interpolation(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;
	
	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;

}




int main()
{
	// Init GLFW
	glfwInit();




	// Set all the required options for GLFW
	/*(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Final Acuario__CGeIHC_Gpo01_Equipo02", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.frag");
	Shader animAgua("Shaders/anim_agua.vs", "Shaders/anim_agua.frag");
	Shader anim2("Shaders/anim2.vs", "Shaders/anim2.frag");

	//**********    Modelos de Tiburones    ****************
	Model tiburonCola((char*)"Models/Tiburon/tiburonCola.obj");
	Model tiburonCuerpo((char*)"Models/Tiburon/tiburonCuerpo.obj");

	Model tiburon2Cola((char*)"Models/Tiburon/colaTib2.obj");
	Model tiburon2Cuerpo((char*)"Models/Tiburon/cuerpoTib2.obj");

	Model tiburon3Cola((char*)"Models/Tiburon/colaTib3.obj");
	Model tiburon3Cuerpo((char*)"Models/Tiburon/cuerpoTib3.obj");

	//**********   Modelos Pescados      **********
	Model pez1((char*)"Models/Fishes/pez1.obj");
	Model pez2((char*)"Models/Fishes/pez2.obj");
	//Model pez2((char*)"Models/Fishes/pex3.obj");
	Model tortuga((char*)"Models/Tortugas/tortuga_marina.obj");
	Model pinguino((char*)"Models/Pinguinos/Penguin1/Penguin.obj");
	Model medusas((char*)"Models/Medusas/medusas.obj");

	//******** Modelo de Tortuga  **********
	Model aletaTDD((char*)"Models/Tortugas/aletaDelanDer.obj");
	Model aletaTDI((char*)"Models/Tortugas/aletaDelanIzq.obj");
	Model aletaTTD((char*)"Models/Tortugas/aletaTrasDer.obj");
	Model aletaTTI((char*)"Models/Tortugas/aletaTrasIzq.obj");
	Model cuerpoTortuga((char*)"Models/Tortugas/cuerpoTortuga.obj");


	//********* Modelos de ambientación
	Model entrada((char*)"Models/Acuario/entrada.obj");
	Model agua((char*)"Models/Acuario/agua.obj");
	Model area_tortuga((char*)"Models/Acuario/Area_Tortugas/area_tortugas.obj");
	Model area_peces((char*)"Models/Acuario/Area_Peces/area_peces.obj");
	Model area_medusas((char*)"Models/Acuario/Area_Medusas/area_medusas.obj");
	Model area_tiburon((char*)"Models/Acuario/Area_Tiburones/area_tiburon.obj");
	Model area_manta((char*)"Models/Acuario/Area_Mantarayas/area_manta.obj");
	Model tiendaRegalos((char*)"Models/Acuario/Tienda_Regalos/tiendaRegalos.obj");

	Model cristales((char*)"Models/Acuario/cristales.obj");
	Model piso((char*)"Models/Acuario/piso.obj");

	// Build and compile our shader program

	//Inicialización de KeyFrames
	
	for(int i=0; i<MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].rotInc = 0;
	}



	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1,&skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	// Load textures
	vector<const GLchar*> faces;

	faces.push_back("SkyBox/front.jpg");
	faces.push_back("SkyBox/back.jpg");
	faces.push_back("SkyBox/top.jpg");
	faces.push_back("SkyBox/bottom.jpg");
	faces.push_back("SkyBox/left.jpg");
	faces.push_back("SkyBox/right.jpg");
	
	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();
		circuitoTiburon1();
		circuitoTiburon2();
		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), luzAmbiental.x, luzAmbiental.y, luzAmbiental.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), luzAmbiental.x, luzAmbiental.y, luzAmbiental.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);


		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), LightP1.x, LightP1.y, LightP1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), LightP1.x, LightP1.y, LightP1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);



		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 1.0f, 0.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 0.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Bind diffuse map
		//glBindTexture(GL_TEXTURE_2D, texture1);*/

		// Bind specular map
		/*glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);*/


		glBindVertexArray(VAO);
		glm::mat4 tmp = glm::mat4(1.0f); //Temp

		//Carga de modelo 

		view = camera.GetViewMatrix();
		glm::mat4 model(1);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "activaTransparencia"), 1);
		glUniform4f(glGetUniformLocation(lightingShader.Program, "colorAlpha"), 1.0, 1.0, 1.0, 1.0);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tortuga.Draw(lightingShader);

		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//pez1.Draw(lightingShader);

		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//pez2.Draw(lightingShader);

		//***********     Dibujo de los tiburones   ***********

		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, posInicT1  + glm::vec3(movTibX, 0, movTibZ));
		model = glm::rotate(model, glm::radians(rotTib), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotCola), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tiburonCola.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, posInicT1 + glm::vec3(movTibX, 0, movTibZ));
		model = glm::rotate(model, glm::radians(rotTib), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tiburonCuerpo.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, posInicT2 + glm::vec3(movTib2X, 0, movTib2Z));
		model = glm::rotate(model, glm::radians(rotTib2), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotCola), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tiburon2Cola.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, posInicT2 + glm::vec3(movTib2X, 0, movTib2Z));
		model = glm::rotate(model, glm::radians(rotTib2), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tiburon2Cuerpo.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, posInicT3 + glm::vec3(movTib2X, 0, movTib2Z));
		model = glm::rotate(model, glm::radians(rotTib2), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotCola), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tiburon3Cola.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, posInicT3 + glm::vec3(movTib2X, 0, movTib2Z));
		model = glm::rotate(model, glm::radians(rotTib2), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tiburon3Cuerpo.Draw(lightingShader);

		//***********     Dibujo de las tortugas   ***********
		model = glm::mat4(1);
		model = glm::translate(model, posInicT1AletaDD);
		//model = glm::rotate(model, glm::radians(rotTib2), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		aletaTDD.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, posInicT1AletaDI);
		//model = glm::rotate(model, glm::radians(rotTib2), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		aletaTDI.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, posInicT1AletaTI);
		//model = glm::rotate(model, glm::radians(rotTib2), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		aletaTTI.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, posInicT1AletaTD);
		//model = glm::rotate(model, glm::radians(rotTib2), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		aletaTTD.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, posInicT1Cuerpo);
		//model = glm::rotate(model, glm::radians(rotTib2), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		cuerpoTortuga.Draw(lightingShader);
		//****************************************************
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		pinguino.Draw(lightingShader);


		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		piso.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		entrada.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		area_peces.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		area_tortuga.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		area_medusas.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		area_tiburon.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		area_manta.Draw(lightingShader);
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tiendaRegalos.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-233.585f, 8.251f, -89.911f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		medusas.Draw(lightingShader);

		//Traslucidez de vidrios de los estanques.

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		////model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		//model = glm::mat4(1);
		////model = glm::scale(model, glm::vec3(1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1f(glGetUniformLocation(lightingShader.Program, "activaTransparencia"), 1);
		//glUniform4f(glGetUniformLocation(lightingShader.Program, "colorAlpha"), 1.0, 1.0, 1.0, 0.4);
		//cristales.Draw(lightingShader);
		//glDisable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST);
		//glBindVertexArray(0);

		//Animación del agua


		animAgua.Use();
		tiempo = glfwGetTime() * speed;
		modelLoc = glGetUniformLocation(animAgua.Program, "model");
		viewLoc = glGetUniformLocation(animAgua.Program, "view");
		projLoc = glGetUniformLocation(animAgua.Program, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::mat4(1);//Seteamos la matriz
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(animAgua.Program, "time"), glfwGetTime());
		agua.Draw(animAgua);

		//Animación medusas
		anim2.Use();
		modelLoc = glGetUniformLocation(anim2.Program, "model");
		viewLoc = glGetUniformLocation(anim2.Program, "view");
		projLoc = glGetUniformLocation(anim2.Program, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::mat4(1);//Seteamos la matriz
		model = glm::translate(model, glm::vec3(-246.211f, 8.251f, -70.706f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(anim2.Program, "time1"), glfwGetTime());
		medusas.Draw(anim2);
		glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);


		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default




		// Swap the screen buffers
		glfwSwapBuffers(window);
	}




	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();




	return 0;
}


void animacion()
{
		//Movimiento del personaje

		if (play)
		{
			if (i_curr_steps >= i_max_steps) //end of animation between frames?
			{
				playIndex++;
				if (playIndex>FrameIndex - 2)	//end of total animation?
				{
					printf("termina anim\n");
					playIndex = 0;
					play = false;
				}
				else //Next frame interpolations
				{
					i_curr_steps = 0; //Reset counter
									  //Interpolation
					interpolation();
				}
			}
			else
			{
				//Draw animation
				posX += KeyFrame[playIndex].incX;
				posY += KeyFrame[playIndex].incY;
				posZ += KeyFrame[playIndex].incZ;

				rotRodIzq += KeyFrame[playIndex].rotInc;

				i_curr_steps++;
			}

		}
	}


void circuitoTiburon1() {
	if (recorridoT1) {
		if (estado1) {
			//colaTiburon1();
			movTibZ += 0.05f;
			if (movTibZ > 45.0) {
				estado1 = false;
				estado2 = true;
			}
		}
		if (estado2) {
			rotTib = 90.0;
			//rotCola = 90.0;
			movTibX += 0.05f;
			if (movTibX > 90) {
				estado2 = false;
				estado3 = true;
			}
		}
		if (estado3) {
			rotTib = 180.0;
			//rotCola = 180.0;
			movTibZ -= 0.05f;
			if (movTibZ < 10) {
				estado3 = false;
				estado4 = true;
			}

		}
		if (estado4) {
			rotTib = 270.0;
			//rotCola = 270.0;
			movTibX -= 0.05f;
			if (movTibX < 0) {
				estado4 = false;
				estado5 = true;
			}
		}
		if (estado5){
			rotTib = 360;
			//rotCola = 360;
			movTibZ += 0.05f;
			if (movTibZ > 0)
			{
				estado5 = false;
				estado1 = true;
			}
		}
		if (izquierda) {
			rotCola += 0.3f;
			if (rotCola > 40) {
				izquierda = false;
				derecha = true;
			}
		}
		if (derecha) {
			rotCola -= 0.3f;
			if (rotCola < (-40)) {
				derecha = false;
				izquierda = true;
			}
		}


	}
}
 
void circuitoTiburon2() {
	if (recorridoT2) {
		if (punto1) {
			movTib2X -= 0.05f;
			if (movTib2X < (-90.0)) {
				punto1 = false;
				punto2 = true;
			}
		}
		if (punto2) {
			rotTib2 = 270.0;
			movTib2Z -= 0.05;
			if (movTib2Z < (-45.0)) {
				punto2 = false;
				punto3 = true;
			}
		}
		if (punto3) {
			rotTib2 = 180.0;
			movTib2X += 0.05;
			if (movTib2X > (-5.0)) {
				punto3 = false;
				punto4 = true;
			}
		}
		if (punto4) {
			rotTib2 = 90.0;
			movTib2Z += 0.05;
			if (movTib2Z > (-8.0)) {
				punto4 = false;
				punto5 = true;
			}
		}
		if (punto5) {
			rotTib2 = 0.0;
			movTib2X -= 0.05f;
			if (movTib2X < 0)
			{
				punto5 = false;
				punto1 = true;
			}
		}

	}
}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_L])
	{
		if (play == false && (FrameIndex > 1))
		{

			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}

	}

	if (keys[GLFW_KEY_K])
	{
		if (FrameIndex<MAX_FRAMES)
		{
			saveFrame();
		}

		rot =-25.0f;//Variable que maneja el giro de la camara

	}


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
			LightP1 = glm::vec3(1.0f, 0.0f, 0.0f);
		else
			LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	//Cambio de luz ambiental
	if (keys[GLFW_KEY_M]) {
		if (cambioAmbiental)
		{
			//Light1 = glm::vec3(0);
			//Light2 = glm::vec3(0);
			//Light3 = glm::vec3(0);
			//Light4 = glm::vec3(0);
			luzAmbiental = glm::vec3(0.5f, 0.5f, 0.5f);
			cambioAmbiental = false;
		}
		else
		{
			//Light1 = glm::vec3(1.0f, 1.0f, 1.0f);
			//Light2 = glm::vec3(1.0f, 1.0f, 1.0f);
			//Light3 = glm::vec3(1.0f, 1.0f, 1.0f);
			//Light4 = glm::vec3(1.0f, 1.0f, 1.0f);
			luzAmbiental = glm::vec3(0.1f, 0.1f, 0.1f);
			cambioAmbiental = true;
		}
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	if (keys[GLFW_KEY_1])
	{
		
		movCamera = 0.01f;//Manda una velocidad de 0.01 a la camara automatica

	}

	if (keys[GLFW_KEY_2])
	{
		if (rotRodIzq<80.0f)
			rotRodIzq += 1.0f;
			
	}

	if (keys[GLFW_KEY_3])
	{
		if (rotRodIzq>-45)
			rotRodIzq -= 1.0f;
		
	}

	

	//Mov Personaje
	if (keys[GLFW_KEY_H])
	{
		posZ += 1;
	}

	if (keys[GLFW_KEY_Y])
	{
		posZ -= 1;
	}

	if (keys[GLFW_KEY_G])
	{
		posX -= 1;
	}

	if (keys[GLFW_KEY_J])
	{
		posX += 1;
	}




	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}






}