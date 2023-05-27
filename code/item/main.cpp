//Below we will make various objects and parameters to tell OpenGL the context it needs during runtime
const char* inf =	"\nQ&A?please call\n"
					"name:biglonglong\n"
					"email:1522262926@qq.com\n";

/* Header File
* base
* function pointers manger
* OpenGL C-interface
* texture loader
* matrix methods
* class:shader、texture、camera
*/
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"
#include "Dependencies/stb_image/stb_image.h"
#include "Dependencies/glm/gtc/matrix_transform.hpp"   
#include "Dependencies/glm/glm.hpp"
#include "Shader.h"
#include "Texture.h"
#include "camera.h"


/* Data Structure
* vertex properties
* model vertex set
* model
* shader
* texture
* camera
*/
struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};
struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};
Model spacecraft, planet, rock, satellite, craft, aircarrier;
Shader ourNormalShader, ourLightShader, ourSkyboxShader, ourSampleShader, ourNormalMapShader;
GLuint skyboxTexture;
Texture spacecraftTexture, planetTexture, planetNormal, rockTexture, goldTexture, satelliteTexture, craftTexture, redTexture, aircarrierTexture;
Camera camera(glm::vec3(0.0f, 0.0f, 1.5f));


/* VAO、VBO、EBO
* [0]  skybox
* [1]  spacecraft
* [2]  planet
* [3]  rockgoldTexture
* [4]  satellite
* [5]  craft
* [6]  aircarrier
*/
GLuint VAO[7];
GLuint VBO[7];
GLuint EBO[7];


/* Variable
* viewport size
* planet rock satellite system
* craft movement
* invin cycle and mount
* customs exit
*/
GLFWwindow* window;  
bool isFullScreen = false;
GLFWmonitor* monitor = NULL;
const int SCR_WIDTH = 960;
const int SCR_HEIGHT = 540;

glm::vec3 system_position = glm::vec3(8.0f, 1.0f, -30.0f);
glm::vec3 rotation_direction = glm::vec3(0.0f, 1.0f, 0.0f);
GLuint rotation_speed = 3.2f;
float rotation_radius = 7.5f;
float rotation_offset = 1.0f;
unsigned int rockmount = 320;
glm::mat4* rockmodelMatrices = new glm::mat4[rockmount];
unsigned int goldmount = 16;
glm::mat4* goldmodelMatrices = new glm::mat4[goldmount];
unsigned int satellitemount = 48;
glm::vec3 satellite_direction[] = { 
	glm::vec3(1.0,0.0,0.0),
	glm::vec3(0.0,1.0,0.0), 
	glm::vec3(0.0,0.0,1.0), 
	glm::vec3(1.0,1.0,0.0), 
	glm::vec3(1.0,0.0,1.0), 
	glm::vec3(0.0,1.0,1.0),
	glm::vec3(1.0,1.0,1.0) 
};

unsigned int craftmount = 16;
GLfloat movement_length = 9.0f;
GLfloat movement_circle = 6.4f;
GLfloat movement_cycle = 0.15f;

double invincible_time = 10.0f;

int golden_mount = 3;
int relive_mount = 3;
glm::vec3 aircarrierPositions = glm::vec3(0.0f, 4.0f, 16.0f);



/* Parameter
* one-render time
* invin clocker
* cursor position
* interactive flag or variable
* mark geted-gold
* customs approach
*/
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

bool continue_flag = 1;												// ENTER:continue when arrested
int cursor_flag = 1;												// F11:cursor out or in
bool edge_flag = 0;													// Q:edging craft's margin
bool light_flag = 1;												// E:open/close flashlight
bool up_rotation_flag = 0;											// W:spacecraft uping
bool down_rotation_flag = 0;										// S:spacecraft downing
bool left_rotation_flag = 0;										// A:spacecraft lefting
bool right_rotation_flag = 0;										// D:spacecraft righting
float speed_limit = 10.0f;											// SHIFT:speeding limit
float direction_rotation_angle = 3.0f;								// SHIFT/Z:speeding/slowing
glm::vec3 view_angle = glm::vec3(0.0f, -0.5f, -1.5f);				// BUTTON_RIGHT:first person or third person
glm::vec3 envir_light = glm::vec3(0.5f);							// F7/F8:enlight/darken environ light
glm::vec3 spot_light = glm::vec3(10.0f);							// +/-:enlight/darken spot light

double invincible_starttime = -invincible_time;

bool goldGeted[500] = { 0 };
int goldgetednum = 0;
int counter = 0;


/*Function
* Get_OpenGL_Info
* ReadTXT
* Clocker
* DistanceCheck
* ArrestLogic
* SucessLogic
* RandrockmodelMatrics
* RandgoldmodelMatrics
* loadOBJ
* loadCubemap
* sendDataToOpenGL:OBJ、VAO、VBO、EBO、Texture
* initializedGL:sendDataToOpenGL、RandrockmodelMatrics、setupShader(samplerNUMBER)、glEnable（GL_DEPTH_TEST、GL_BLEND、GL_CULL_FACE）
* paintGL:FOR YOU！！！（projection、view、model、uniform in shader）
*/
void printauthorinf(const char* s) {
	printf("\033[0m\033[1;36m%s\033[0m", s);
}

void Get_OpenGL_Info() {
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	std::cout << "OpenGL company: " << name << std::endl;
	std::cout << "Renderer name: " << renderer << std::endl;
	std::cout << "OpenGL version: " << glversion << std::endl;
}

void ReadTXT(std::string filename) {
	std::fstream file;
	file.open(filename, std::ios::in);
	if (file.is_open()) {
		std::string line;
		while (getline(file, line)) {
			std::cout << line << std::endl;
		}
		file.close();
	}
}

bool Clocker(double starttime, double cycle) {
	return glfwGetTime() - starttime - cycle > 0 ? 0 : 1;
}

int DistanceCheck(glm::vec3 originpos, glm::mat4 model, float threshold1, float threshold2) {
	float distance = sqrt(pow(originpos.x - model[3][0], 2) + pow(originpos.y - model[3][1], 2) + pow(originpos.z - model[3][2], 2));
	if (distance >= threshold1) return 0;
	else if (distance >= threshold2) return 1;
	else if (distance < threshold2) return -1;
	else {
		std::cout << "there are some error in model or origin.check:" << std::endl;
		std::cout << "model.x" << '\t' << "model.y" << '\t' << "model.z" << '\t' << "origin.x" << '\t' << "origin.y" << '\t' << "origin.z" << std::endl;
		std::cout << model[3][0] << '\t' << model[3][1] << '\t' << model[3][2] << '\t' << originpos.x << '\t' << originpos.y << '\t' << originpos.z << std::endl;
		return -2;
	}
}

void ArrestLogic() {
	if (relive_mount == 0) {
		system("cls");
		ReadTXT(std::string("fuck.txt"));
		std::cout << "You are under arrest by Starcraft Police! ! !" << std::endl;
		std::cout << "You are under arrest by Starcraft Police! ! !" << std::endl;
		std::cout << "You are under arrest by Starcraft Police! ! !" << std::endl;
		std::cout << "You are under arrest by Starcraft Police! ! !" << std::endl;
		std::cout << "Caution: Flashlight (E)!!!" << std::endl;
		std::cout << "Caution:   radar (Q)   !!!" << std::endl;
		std::cout << "Caution:   Warning     !!!" << std::endl;
		std::cout << "Please start again!!!!" << std::endl;
		std::cout << "Please start again!!!!" << std::endl;
		std::cout << "Please start again!!!!" << std::endl;
		glfwDestroyWindow(window);
	}

	continue_flag = 0;
	while (!continue_flag) {
		glfwPollEvents();
	};
	invincible_starttime = glfwGetTime();
}

void SucessLogic() {
	system("cls");
	ReadTXT("over.txt");
	std::cout << "Congratulations, you made it! ! !" << std::endl;
	std::cout << "Congratulations, you made it! ! !" << std::endl;
	std::cout << "Congratulations, you made it! ! !" << std::endl;
	std::cout << "Congratulations, you made it! ! !" << std::endl;
	std::cout << "else:" << std::endl;
	std::cout << "I know the code has a lot of shortcomings, please question and discussion by! ! !" << std::endl;
	printauthorinf(inf);
	glfwDestroyWindow(window);
}

void RandrockmodelMatrics(glm::mat4* rockmodelMatrices, unsigned int rockmount) {
	srand(static_cast<unsigned int>(glfwGetTime()));
	for (unsigned int i = 0; i < rockmount; i++) {
		float displacement = (rand() % (int)(2 * rotation_offset * 100)) / 100.0f - rotation_offset;  //displacement in [-offset,offset] regulatly

		glm::mat4 modelsample = glm::translate(glm::mat4(1.0f), 
			glm::vec3(sin(glm::radians(float(i))) * rotation_radius + displacement, displacement, cos(glm::radians(float(i)))*rotation_radius + displacement));
		modelsample = glm::rotate(modelsample, glm::radians(static_cast<float>((rand() % 360))), glm::vec3(0.4f, 0.6f, 0.8f));
		float scale = static_cast<float>((rand() % 50) / 1000.0 + 0.02);
		modelsample = glm::scale(modelsample, glm::vec3(scale));
		rockmodelMatrices[i] = modelsample;
	}
}

void RandgoldmodelMatrics(glm::mat4* goldmodelMatrices, unsigned int goldmount) {
	srand(static_cast<unsigned int>(glfwGetTime()));
	for (unsigned int i = 0; i < goldmount; i++) {
		float displacement = 2 * i * rotation_offset / goldmount - rotation_offset;   ////displacement in [-offset,offset] unregulatly

		glm::mat4 modelsample = glm::translate(glm::mat4(1.0f), 
			system_position + glm::vec3(displacement));
		modelsample = glm::rotate(modelsample, glm::radians(rotation_speed * static_cast<float>(glfwGetTime())), rotation_direction);
		modelsample = glm::translate(modelsample,
			glm::vec3(sin(float(i)) * rotation_radius + displacement, displacement, cos(float(i)) * rotation_radius + displacement));
		modelsample = glm::scale(modelsample, glm::vec3(0.1 / i + 0.02));
		goldmodelMatrices[i] = modelsample;

	}
}

Model loadOBJ(const char* objPath)
{
	// function to load the obj file
	// Note: this simple function cannot load all obj files.

	struct V {
		// struct for identify if a vertex has showed up
		unsigned int index_position, index_uv, index_normal;
		bool operator == (const V& v) const {
			return index_position == v.index_position && index_uv == v.index_uv && index_normal == v.index_normal;
		}
		bool operator < (const V& v) const {
			return (index_position < v.index_position) ||
				(index_position == v.index_position && index_uv < v.index_uv) ||
				(index_position == v.index_position && index_uv == v.index_uv && index_normal < v.index_normal);
		}
	};

	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::map<V, unsigned int> temp_vertices;

	Model model;
	unsigned int num_vertices = 0;

	std::cout << "\nLoading OBJ file " << objPath << "..." << std::endl;

	std::ifstream file;
	file.open(objPath);

	// Check for Error
	if (file.fail()) {
		std::cerr << "Impossible to open the file! Do you use the right path? See Tutorial 6 for details" << std::endl;
		exit(1);
	}

	while (!file.eof()) {
		// process the object file
		char lineHeader[128];
		file >> lineHeader;

		if (strcmp(lineHeader, "v") == 0) {
			// geometric vertices
			glm::vec3 position;
			file >> position.x >> position.y >> position.z;
			temp_positions.push_back(position);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			// texture coordinates
			glm::vec2 uv;
			file >> uv.x >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			// vertex normals
			glm::vec3 normal;
			file >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			// Face elements
			V vertices[3];
			for (int i = 0; i < 3; i++) {
				char ch;
				file >> vertices[i].index_position >> ch >> vertices[i].index_uv >> ch >> vertices[i].index_normal;
			}

			// Check if there are more than three vertices in one face.
			std::string redundency;
			std::getline(file, redundency);
			if (redundency.length() >= 5) {
				std::cerr << "There may exist some errors while load the obj file. Error content: [" << redundency << " ]" << std::endl;
				std::cerr << "Please note that we only support the faces drawing with triangles. There are more than three vertices in one face." << std::endl;
				std::cerr << "Your obj file can't be read properly by our simple parser :-( Try exporting with other options." << std::endl;
				exit(1);
			}

			for (int i = 0; i < 3; i++) {
				if (temp_vertices.find(vertices[i]) == temp_vertices.end()) {
					// the vertex never shows before
					Vertex vertex;
					vertex.position = temp_positions[vertices[i].index_position - 1];
					vertex.uv = temp_uvs[vertices[i].index_uv - 1];
					vertex.normal = temp_normals[vertices[i].index_normal - 1];

					model.vertices.push_back(vertex);
					model.indices.push_back(num_vertices);
					temp_vertices[vertices[i]] = num_vertices;
					num_vertices += 1;
				}
				else {
					// reuse the existing vertex
					unsigned int index = temp_vertices[vertices[i]];
					model.indices.push_back(index);
				}
			} // for
		} // else if
		else {
			// it's not a vertex, texture coordinate, normal or face
			char stupidBuffer[1024];
			file.getline(stupidBuffer, 1024);
		}
	}
	file.close();

	std::cout << "There are " << num_vertices << " vertices in the obj file.\n" << std::endl;
	return model;
}

GLuint loadCubemap(std::vector<std::string> faces) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void sendDataToOpenGL() {
	//load model
	GLfloat skyboxVertices[] = {
		// positions          
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
	spacecraft = loadOBJ("resources/spacecraft/spacecraft.obj");
	planet = loadOBJ("resources/planet/planet.obj");
	rock = loadOBJ("resources/rock/rock.obj");
	satellite = loadOBJ("resources/satellite/satellite.obj");
	craft = loadOBJ("resources/craft/craft.obj");
	aircarrier = loadOBJ("resources/aircarrier/aircarrier.obj");

	//VAO bound VBO、EBO
	glGenVertexArrays(1, &VAO[0]);
	glBindVertexArray(VAO[0]);
	glGenBuffers(1, &VBO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &VAO[1]);
	glBindVertexArray(VAO[1]);
	glGenBuffers(1, &VBO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, spacecraft.vertices.size() * sizeof(Vertex), &spacecraft.vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &EBO[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, spacecraft.indices.size() * sizeof(unsigned int), &spacecraft.indices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &VAO[2]);
	glBindVertexArray(VAO[2]);
	glGenBuffers(1, &VBO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, planet.vertices.size() * sizeof(Vertex), &planet.vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &EBO[2]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, planet.indices.size() * sizeof(unsigned int), &planet.indices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &VAO[3]);
	glBindVertexArray(VAO[3]);
	glGenBuffers(1, &VBO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, rock.vertices.size() * sizeof(Vertex), &rock.vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &EBO[3]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, rock.indices.size() * sizeof(unsigned int), &rock.indices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &VAO[4]);
	glBindVertexArray(VAO[4]);
	glGenBuffers(1, &VBO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, satellite.vertices.size() * sizeof(Vertex), &satellite.vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &EBO[4]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[4]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, satellite.indices.size() * sizeof(unsigned int), &satellite.indices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &VAO[5]);
	glBindVertexArray(VAO[5]);
	glGenBuffers(1, &VBO[5]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
	glBufferData(GL_ARRAY_BUFFER, craft.vertices.size() * sizeof(Vertex), &craft.vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &EBO[5]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[5]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, craft.indices.size() * sizeof(unsigned int), &craft.indices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &VAO[6]);
	glBindVertexArray(VAO[6]);
	glGenBuffers(1, &VBO[6]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
	glBufferData(GL_ARRAY_BUFFER, aircarrier.vertices.size() * sizeof(Vertex), &aircarrier.vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &EBO[6]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[6]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, aircarrier.indices.size() * sizeof(unsigned int), &aircarrier.indices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	// Establish texture objects
	std::vector<std::string> faces{
		"resources/skybox/right.bmp",
		"resources/skybox/left.bmp",
		"resources/skybox/top.bmp",
		"resources/skybox/bottom.bmp",
		"resources/skybox/front.bmp",
		"resources/skybox/back.bmp"
	};
	skyboxTexture = loadCubemap(faces);
	spacecraftTexture.setupTexture("resources/spacecraft/spacecraftTexture.bmp");
	planetTexture.setupTexture("resources/planet/planetTexture.bmp");
	planetNormal.setupTexture("resources/planet/planetNormal.bmp");
	rockTexture.setupTexture("resources/rock/rockTexture.jpg");
	goldTexture.setupTexture("resources/rock/goldTexture.png");
	satelliteTexture.setupTexture("resources/satellite/satelliteTexture.jpg");
	craftTexture.setupTexture("resources/craft/craftTexture.png");
	redTexture.setupTexture("resources/craft/redTexture.png");
	aircarrierTexture.setupTexture("resources/aircarrier/aircarrierTexture.png");
}

void initializedGL(void) {
	Get_OpenGL_Info();
	sendDataToOpenGL();
	RandrockmodelMatrics(rockmodelMatrices, rockmount);

	ourNormalShader.setupShader("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
	ourLightShader.setupShader("VertexShaderCodeLight.glsl", "FragmentShaderCodeLight.glsl");
	ourSkyboxShader.setupShader("VertexShaderCodeSkyBox.glsl", "FragmentShaderCodeSkyBox.glsl");
	ourSampleShader.setupShader("VertexShaderCodeSample.glsl", "FragmentShaderCodeSample.glsl");
	ourNormalMapShader.setupShader("VertexShaderCodeNormal.glsl", "FragmentShaderCodeNormal.glsl");

	ourLightShader.use();
	ourLightShader.setInt("material.diffuse", 0);
	ourLightShader.setInt("material.specular", 1);
	ourNormalMapShader.use();
	ourNormalMapShader.setInt("material.diffuse", 0);
	ourNormalMapShader.setInt("material.diffuse", 1);
	ourNormalMapShader.setInt("normalMap", 2);
	ourSkyboxShader.use();
	ourSkyboxShader.setInt("skybox", 0);
	ourSampleShader.use();
	ourSampleShader.setInt("diffuse", 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// glDepthMask(GL_FALSE);  

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
}

void paintGL(void) {
	//Set the background
	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Unified perspective and perspective
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	ourSkyboxShader.use();
	ourSkyboxShader.setMat4("projection", projection);
	glm::mat4 skyboxview = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	ourSkyboxShader.setMat4("view", skyboxview);

	ourNormalShader.use();
	ourNormalShader.setMat4("projection", projection);
	ourNormalShader.setMat4("view", view);

	ourSampleShader.use();
	ourSampleShader.setMat4("projection", projection);
	ourSampleShader.setMat4("view", view);
	for (unsigned int i = 0; i < rockmount; i++) {
		std::stringstream ss;
		std::string index;
		ss << i;
		index = ss.str();
		ourSampleShader.setMat4(("aInstanceMatrix[" + index + "]").c_str(), rockmodelMatrices[i]);
	}

	ourLightShader.use();
	ourLightShader.setMat4("projection", projection);
	ourLightShader.setMat4("view", view);
	ourLightShader.setVec3("viewPos", camera.Position);
	ourLightShader.setFloat("material.shininess", 32.0f);
	// dirLight
	ourLightShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	ourLightShader.setVec3("dirLight.ambient", envir_light);
	ourLightShader.setVec3("dirLight.diffuse", envir_light);
	ourLightShader.setVec3("dirLight.specular", envir_light);
	glm::vec3 PointlightPositions[] = {
		glm::vec3(3.0f,  0.0f, 3.0f),
		glm::vec3(3.0f, 0.0f, -3.0f),
		glm::vec3(-3.0f,  0.0f, 3.0f),
		glm::vec3(-3.0f,  0.0f, -3.0f)
	};
	// point light 1
	ourLightShader.setVec3("pointLights[0].position", PointlightPositions[0]);
	ourLightShader.setVec3("pointLights[0].ambient", 0.01f, 0.01f, 0.01f);
	ourLightShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	ourLightShader.setVec3("pointLights[0].specular", 10.0f, 10.0f, 10.0f);
	ourLightShader.setFloat("pointLights[0].constant", 1.0f);
	ourLightShader.setFloat("pointLights[0].linear", 0.09f);
	ourLightShader.setFloat("pointLights[0].quadratic", 0.032f);
	// point light 2
	ourLightShader.setVec3("pointLights[1].position", PointlightPositions[1]);
	ourLightShader.setVec3("pointLights[1].ambient", 0.01f, 0.01f, 0.01f);
	ourLightShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	ourLightShader.setVec3("pointLights[1].specular", 10.0f, 10.0f, 10.0f);
	ourLightShader.setFloat("pointLights[1].constant", 1.0f);
	ourLightShader.setFloat("pointLights[1].linear", 0.09f);
	ourLightShader.setFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	ourLightShader.setVec3("pointLights[2].position", PointlightPositions[2]);
	ourLightShader.setVec3("pointLights[2].ambient", 0.01f, 0.01f, 0.01f);
	ourLightShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	ourLightShader.setVec3("pointLights[2].specular", 10.0f, 10.0f, 10.0f);
	ourLightShader.setFloat("pointLights[2].constant", 1.0f);
	ourLightShader.setFloat("pointLights[2].linear", 0.09f);
	ourLightShader.setFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	ourLightShader.setVec3("pointLights[3].position", PointlightPositions[3]);
	ourLightShader.setVec3("pointLights[3].ambient", 0.01f, 0.01f, 0.01f);
	ourLightShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	ourLightShader.setVec3("pointLights[3].specular", 10.0f, 10.0f, 10.0f);
	ourLightShader.setFloat("pointLights[3].constant", 1.0f);
	ourLightShader.setFloat("pointLights[3].linear", 0.09f);
	ourLightShader.setFloat("pointLights[3].quadratic", 0.032f);
	// spotLight
	ourLightShader.setVec3("spotLight.position", view_angle);
	ourLightShader.setVec3("spotLight.direction", camera.Front);
	if (light_flag) {
		ourLightShader.setVec3("spotLight.ambient", spot_light);	
	}
	else {
		ourLightShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	}
	ourLightShader.setVec3("spotLight.diffuse", 20.0f, 20.0f, 20.0f);
	ourLightShader.setVec3("spotLight.specular", 10.0f, 10.0f, 10.0f);
	ourLightShader.setFloat("spotLight.constant", 1.0f);
	ourLightShader.setFloat("spotLight.linear", 0.09f);
	ourLightShader.setFloat("spotLight.quadratic", 0.032f);
	ourLightShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	ourLightShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	ourNormalMapShader.use();
	ourNormalMapShader.setMat4("projection", projection);
	ourNormalMapShader.setMat4("view", view);
	ourNormalMapShader.setVec3("viewPos", camera.Position);
	ourNormalMapShader.setFloat("material.shininess", 8.0f);
	// dirLight
	ourNormalMapShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	ourNormalMapShader.setVec3("dirLight.ambient", envir_light);
	ourNormalMapShader.setVec3("dirLight.diffuse", envir_light);
	ourNormalMapShader.setVec3("dirLight.specular", envir_light);
	// point light 1
	ourNormalMapShader.setVec3("pointLights[0].position", PointlightPositions[0]);
	ourNormalMapShader.setVec3("pointLights[0].ambient", 0.2f, 0.2f, 0.2f);
	ourNormalMapShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	ourNormalMapShader.setVec3("pointLights[0].specular", 10.0f, 10.0f, 10.0f);
	ourNormalMapShader.setFloat("pointLights[0].constant", 1.0f);
	ourNormalMapShader.setFloat("pointLights[0].linear", 0.09f);
	ourNormalMapShader.setFloat("pointLights[0].quadratic", 0.032f);
	// point light 2
	ourNormalMapShader.setVec3("pointLights[1].position", PointlightPositions[1]);
	ourNormalMapShader.setVec3("pointLights[1].ambient", 0.2f, 0.2f, 0.2f);
	ourNormalMapShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	ourNormalMapShader.setVec3("pointLights[1].specular", 10.0f, 10.0f, 10.0f);
	ourNormalMapShader.setFloat("pointLights[1].constant", 1.0f);
	ourNormalMapShader.setFloat("pointLights[1].linear", 0.09f);
	ourNormalMapShader.setFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	ourNormalMapShader.setVec3("pointLights[2].position", PointlightPositions[2]);
	ourNormalMapShader.setVec3("pointLights[2].ambient", 0.01f, 0.01f, 0.01f);
	ourNormalMapShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	ourNormalMapShader.setVec3("pointLights[2].specular", 10.0f, 10.0f, 10.0f);
	ourNormalMapShader.setFloat("pointLights[2].constant", 1.0f);
	ourNormalMapShader.setFloat("pointLights[2].linear", 0.09f);
	ourNormalMapShader.setFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	ourNormalMapShader.setVec3("pointLights[3].position", PointlightPositions[3]);
	ourNormalMapShader.setVec3("pointLights[3].ambient", 0.01f, 0.01f, 0.01f);
	ourNormalMapShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	ourNormalMapShader.setVec3("pointLights[3].specular", 10.0f, 10.0f, 10.0f);
	ourNormalMapShader.setFloat("pointLights[3].constant", 1.0f);
	ourNormalMapShader.setFloat("pointLights[3].linear", 0.09f);
	ourNormalMapShader.setFloat("pointLights[3].quadratic", 0.032f);
	// spotLight
	ourNormalMapShader.setVec3("spotLight.position", view_angle);
	ourNormalMapShader.setVec3("spotLight.direction", camera.Front);
	if (light_flag) {
		ourNormalMapShader.setVec3("spotLight.ambient", spot_light);
	}
	else {
		ourNormalMapShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	}
	ourNormalMapShader.setVec3("spotLight.diffuse", 20.0f, 20.0f, 20.0f);
	ourNormalMapShader.setVec3("spotLight.specular", 10.0f, 10.0f, 10.0f);
	ourNormalMapShader.setFloat("spotLight.constant", 1.0f);
	ourNormalMapShader.setFloat("spotLight.linear", 0.09f);
	ourNormalMapShader.setFloat("spotLight.quadratic", 0.032f);
	ourNormalMapShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	ourNormalMapShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));



	// Set up model personalization:model、Tex.bind、VAO.bind
	for (unsigned int i = 0; i < 7; i++) {
		if (i == 0) { //skybox
			glDepthFunc(GL_LEQUAL);

			ourSkyboxShader.use();
			glBindVertexArray(VAO[0]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			glDepthFunc(GL_LESS);
		}

		else if (i == 1) { //spacecraft
			ourLightShader.use();
			glBindVertexArray(VAO[1]);
			spacecraftTexture.bind(1);
			spacecraftTexture.bind(0);
			if (Clocker(invincible_starttime, invincible_time)) {
				craftTexture.bind(0);
			}
			if (goldgetednum>=golden_mount) {
				goldTexture.bind(0);
			}
			view = glm::mat4(1.0f);
			ourLightShader.setMat4("view", view);
			model = glm::translate(glm::mat4(1.0f), view_angle);
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			if (left_rotation_flag) {
				model = glm::rotate(model, glm::radians(direction_rotation_angle), glm::vec3(0.0f, 0.0f, -1.0f));
			}	
			if (right_rotation_flag) {
				model = glm::rotate(model, glm::radians(direction_rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f));
			}	
			if (up_rotation_flag) {
				model = glm::rotate(model, glm::radians(direction_rotation_angle), glm::vec3(-1.0f, 0.0f, 0.0f));
			}
			if (down_rotation_flag) {
				model = glm::rotate(model, glm::radians(direction_rotation_angle), glm::vec3(1.0f, 0.0f, 0.0f));
			}	
			model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
			ourLightShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, spacecraft.indices.size(), GL_UNSIGNED_INT, 0);

			view = camera.GetViewMatrix();
			ourLightShader.setMat4("view", view);
		}

		else if (i == 2) { //planet
			ourNormalMapShader.use();
			glBindVertexArray(VAO[2]);
			planetTexture.bind(0);
			planetTexture.bind(1);
			planetNormal.bind(2);
			model = glm::translate(glm::mat4(1.0f), glm::vec3(system_position));
			model = glm::rotate(model, glm::radians(rotation_speed * static_cast<float>(glfwGetTime())), rotation_direction);
			model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
			ourNormalMapShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, planet.indices.size(), GL_UNSIGNED_INT, 0);
		}

		else if (i == 3) { //rock & gold
			ourSampleShader.use();
			glBindVertexArray(VAO[3]);
			rockTexture.bind(0);
			model = glm::translate(glm::mat4(1.0f), system_position);
			model = glm::rotate(model, glm::radians(rotation_speed * static_cast<float>(glfwGetTime())), rotation_direction);
			ourSampleShader.setMat4("model", model);
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rock.indices.size()), GL_UNSIGNED_INT, 0, rockmount);
			
			ourLightShader.use();
			glBindVertexArray(VAO[3]);
			goldTexture.bind(0);
			RandgoldmodelMatrics(goldmodelMatrices, goldmount);
			for (int goldnum = 0; goldnum < goldmount; goldnum++) {
				if (goldGeted[goldnum]) continue;
				model = goldmodelMatrices[goldnum];
				ourLightShader.setMat4("model", model);
				float distance = DistanceCheck(camera.Position, model, 1.0, 1.0);
				if (distance==-1) {
					goldGeted[goldnum] = 1;
					goldgetednum++;
				}
				glDrawElements(GL_TRIANGLES, rock.indices.size(), GL_UNSIGNED_INT, 0);
			}
		}

		else if (i == 4) {  //satellite
			glBindVertexArray(VAO[4]);
			satelliteTexture.bind(0);
			satelliteTexture.bind(1);
			int sign = 1;
			for (int satellitenum = 0; satellitenum < satellitemount; satellitenum++, sign = -sign) {
				int number = satellitenum % (sizeof(satellite_direction) / sizeof(satellite_direction[0]));
				model = glm::translate(glm::mat4(1.0f), glm::vec3(system_position));
				model = glm::rotate(model, glm::radians(rotation_speed * 10 * static_cast<float>(glfwGetTime())), satellite_direction[number]);
				model = glm::translate(model, glm::vec3(sin(glm::radians(360.0f-360.0f/satellitemount * satellitenum)) * 1.5 * rotation_radius, sign*0.12f * satellitenum, cos(glm::radians(360.0f - 360.0f / satellitemount *satellitenum)) * 1.5 * rotation_radius));
				model = glm::rotate(model, glm::radians(rotation_speed * 30 * static_cast<float>(glfwGetTime())), system_position - glm::vec3(model[3][0], model[3][1], model[3][2]));
				model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
				ourLightShader.setMat4("model", model);
				glDrawElements(GL_TRIANGLES, craft.indices.size(), GL_UNSIGNED_INT, 0);
			}
		}

		else if (i == 5) {  //aircarrier
			ourLightShader.use();
			glBindVertexArray(VAO[6]);
			aircarrierTexture.bind(0);
			aircarrierTexture.bind(1);
			model = glm::translate(glm::mat4(1.0f), aircarrierPositions);
			model = glm::rotate(model, glm::radians(float(glfwGetTime()*20)), glm::vec3(0.0f,1.0f,0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			ourLightShader.setMat4("model", model);
			float distance = DistanceCheck(camera.Position, model, 10.0, 8.0);
			if (distance == -1 && goldgetednum>=golden_mount) {
				view_angle += glm::vec3(0.0f,0.01f,0.0f);
				counter++;
				if (counter >= 100) {
					SucessLogic();
				} 
			}
			glDrawElements(GL_TRIANGLES, aircarrier.indices.size(), GL_UNSIGNED_INT, 0);
		}
		
		else {  //craft
			glEnable(GL_STENCIL_TEST);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

			glBindVertexArray(VAO[5]);
			for (int craftnum = 0; craftnum < craftmount; craftnum++) {
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);

				ourLightShader.use();
				craftTexture.bind(0);
				craftTexture.bind(1);
				model = glm::translate(glm::mat4(1.0f), 
					glm::vec3(movement_circle * cos(movement_cycle*5*glfwGetTime()+craftnum), movement_circle * sin(movement_cycle*5*glfwGetTime()+craftnum),-movement_length+movement_length *sin(movement_cycle*(glfwGetTime()+craftnum))));
				model = glm::rotate(model, glm::radians(rotation_speed * 10 * static_cast<float>(glfwGetTime())), glm::vec3(0.0, 1.0, 0.0));
				model = glm::scale(model, glm::vec3(0.1f));
				ourLightShader.setMat4("model", model);
				int distance = DistanceCheck(camera.Position, model, 5.5, 2.5);
				if (distance ==(1 ||-1)) {
					redTexture.bind(0);
				}
				if (distance == -1 && !Clocker(invincible_starttime, invincible_time)) {
					relive_mount--;
					ArrestLogic();
				}
				glDrawElements(GL_TRIANGLES, craft.indices.size(), GL_UNSIGNED_INT, 0);

				if (edge_flag) {
					glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
					glStencilMask(0x00);
					glDisable(GL_DEPTH_TEST);

					ourNormalShader.use();
					glBindVertexArray(VAO[5]);
					model = glm::translate(glm::mat4(1.0f),
						glm::vec3(movement_circle * cos(movement_cycle * 5 * glfwGetTime() + craftnum), movement_circle * sin(movement_cycle * 5 * glfwGetTime() + craftnum), -movement_length + movement_length * sin(movement_cycle * (glfwGetTime() + craftnum))));
					model = glm::rotate(model, glm::radians(rotation_speed * 10 * static_cast<float>(glfwGetTime())), glm::vec3(0.0, 1.0, 0.0));
					model = glm::scale(model, glm::vec3(0.1f));
					ourNormalShader.setMat4("model", model);
					glDrawElements(GL_TRIANGLES, craft.indices.size(), GL_UNSIGNED_INT, 0);

					glEnable(GL_DEPTH_TEST);
				}
			}
			glDisable(GL_STENCIL_TEST);
		}
	}
	glBindVertexArray(0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		system("cls");
		ReadTXT(std::string("fuck.txt"));
		std::cout << "You are under arrest by Starcraft Police! ! !" << std::endl;
		std::cout << "You are under arrest by Starcraft Police! ! !" << std::endl;
		std::cout << "You are under arrest by Starcraft Police! ! !" << std::endl;
		std::cout << "You are under arrest by Starcraft Police! ! !" << std::endl;
		std::cout << "Caution: Flashlight (E)!!!" << std::endl;
		std::cout << "Caution:   radar (Q)   !!!" << std::endl;
		std::cout << "Caution:   Warning     !!!" << std::endl;
		std::cout << "Please start again!!!!" << std::endl;
		std::cout << "Please start again!!!!" << std::endl;
		std::cout << "Please start again!!!!" << std::endl;
		printauthorinf(inf);
		glfwDestroyWindow(window);
	}
	if (key == GLFW_KEY_A && action == GLFW_REPEAT) {
		camera.ProcessKeyboard(LEFT, deltaTime);
		left_rotation_flag = 1;
	}
	if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		left_rotation_flag = 0;
	}
	if (key == GLFW_KEY_D && action == GLFW_REPEAT) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
		right_rotation_flag = 1;
	}
	if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		right_rotation_flag = 0;
	}
	if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
		up_rotation_flag = 1;
	}
	if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		up_rotation_flag = 0;
	}
	if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		down_rotation_flag = 1;
	}
	if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		down_rotation_flag = 0;
	}
	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		edge_flag = !edge_flag;
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		light_flag = !light_flag;
	}
	if (key == GLFW_KEY_F8 && action == GLFW_PRESS) {
		envir_light += glm::vec3(0.015f);
	}
	if (key == GLFW_KEY_F7 && action == GLFW_PRESS) {
		envir_light -= glm::vec3(0.015f);
		if (envir_light == glm::vec3(0.0f)) {
			envir_light = glm::vec3(0.0f);
		}
	}
	if (key == GLFW_KEY_F10 && action == GLFW_PRESS) {
		if (cursor_flag==1) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			cursor_flag = 2;
		}
		else if (cursor_flag == 2) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cursor_flag = 0;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cursor_flag = 1;
		}
	}
	if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
		if (isFullScreen) {
			glfwSetWindowMonitor(window, nullptr, SCR_WIDTH/2, SCR_HEIGHT/2, SCR_WIDTH, SCR_HEIGHT, 0);
			isFullScreen = 0;
		}
		else {
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
			isFullScreen = 1;
		}
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		if (direction_rotation_angle != speed_limit) {
			direction_rotation_angle += 1.0f;
			camera.MovementSpeed = 10.0f + direction_rotation_angle * 5.0f;
		}
	}
	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		if (direction_rotation_angle != 1.0f) {
			direction_rotation_angle -= 1.0f;
			camera.MovementSpeed = 10.0f + direction_rotation_angle * 5.0f;
		}
	}
	if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS) {
		spot_light += glm::vec3(1.0f);
	}
	if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) {
		spot_light -= glm::vec3(1.0f);
		if (spot_light == glm::vec3(0.0f)) {
			spot_light = glm::vec3(0.0f);
		}
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		memset(goldGeted, 0, sizeof(goldGeted));
		goldgetednum = 0;
		std::cout << "all golds get in space!!!" << std::endl;
	}
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		continue_flag = 1;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (view_angle == glm::vec3(0.0f, -0.5f, -1.5f)) {
			view_angle = glm::vec3(0.0f, -0.205f, -0.008f);
		}
		else {
			view_angle = glm::vec3(0.0f, -0.5f, -1.5f);
		}
	}
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
	float xpos = static_cast<float>(x);
	float ypos = static_cast<float>(y);
	float sensitivity = 0.1f;
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//camera.ProcessMouseScroll(static_cast<float>(yoffset));
	camera.MouseSensitivity += 0.002*yoffset;
	if (camera.MouseSensitivity < 0) {
		camera.MouseSensitivity = 0;
	}
}

int main(int argc, char* argv[]) {
	// glfw initialization
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	monitor = glfwGetPrimaryMonitor();
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Space Treasure Hunt.", NULL, NULL);
	glfwSetWindowMonitor(window, nullptr, SCR_WIDTH / 2, SCR_HEIGHT / 2, SCR_WIDTH, SCR_HEIGHT, 0);

	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//glew initialization
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//Initialize the OpenGL context
	initializedGL();

	//Loop rendering
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		paintGL();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}