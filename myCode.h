#pragma once
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad\glad.h>
#include "MyShader.h"
#include "MyComputeShader.h"
#include "src/MyPoissonSample.h"
#include <glm/gtx/quaternion.hpp>

#include "src\MyCameraManager.h"
#include "src\MyImGuiPanel.h"



#include <glm/gtc/type_ptr.hpp>

// shader 
MyShader* myShaderPointer;
MyComputeShader* myCompShaderPointer;
MyComputeShader* myResetCompShaderPointer;

// load model
std::vector<float> vertexData;
std::vector<float> uvData;
std::vector<float> normalData;
std::vector<unsigned int> indices;
int vertexCount[5] = { 0 };
int indexCount[5] = { 0 };


// SSBO : draw commands, for compute instanceCount, pass it to compute shader
struct DrawElementsIndirectCommand {
	unsigned int count;
	unsigned int instanceCount;
	unsigned int firstIndex;
	unsigned int baseVertex;
	unsigned int baseInstance;
};
DrawElementsIndirectCommand drawCommands[5];

// SSBO : position, for culling 
struct InstanceProperties {
	glm::vec4 position;
};
InstanceProperties* rawInsData;


// buffer
GLuint vaoHandle;       // attribute
GLuint vboHandle;       // vertex
GLuint uvboHandle;      // texture coordinate
GLuint nboHandle;       // normal 
GLuint iboHandle;       // index
GLuint cmdBufferHandle; // draw command, for indirect drawing, pass it to vertex shader

int NUM_TOTAL_INSTANCE = 0;
int NUM_SAMPLE0 = 0;
int NUM_SAMPLE1 = 0;
int NUM_SAMPLE2 = 0;
int NUM_SAMPLE3 = 0;
int NUM_SAMPLE4 = 0;

// uniform location
GLuint viewProjMatLoc; // for culling, only from player camera
GLuint viewMatLoc;     // rotate, identity
GLuint projMatLoc;     // projective, from player/god camera
GLuint modelMatLoc;    // model, from player/godd camera
GLuint albedoLoc;      // texture image

// load texture
const int NUM_TEXTURE = 5;
const int IMG_WIDTH = 1024;
const int IMG_HEIGHT = 1024;
const int IMG_CHANNEL = 4;
unsigned char* textureArrayData = new unsigned char[IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL * NUM_TEXTURE]; // texture data
GLuint textureArrayHandle; // texture buffer




// rock

// shader  
MyShader* rockShaderPointer;

MyShader* planeShaderPointer;

// load model 
std::vector<float> rockVertexData;
std::vector<float> rockUvData;
std::vector<float> rockNormalData;
std::vector<unsigned int> rockIndices;
std::vector<float> rockTangent;
std::vector<unsigned int> rockBitangent;

int rockIndexCount = 0;
int rockVertexCount = 0;

std::vector<float> planeVertexData;
std::vector<float> planeUvData;
std::vector<float> planeNormalData;
std::vector<unsigned int> planeIndices;

int planeIndexCount = 0;
int planeVertexCount = 0;

// load texture
unsigned char* rockTextureData;
GLuint rockTextureHandle;

unsigned char* rockNormalTextureData;
GLuint rockNormalTextureHandle;

unsigned char* planeTextureData;
GLuint planeTextureHandle;

// buffer
GLuint rockVaoHandle;       // attribute
GLuint rockVboHandle;       // vertex
GLuint rockUvboHandle;      // texture coordinate
GLuint rockNboHandle;       // normal 
GLuint rockTboHandle;       // tangent 
GLuint rockIboHandle;       // index

GLuint planeVaoHandle;       // attribute
GLuint planeVboHandle;       // vertex
GLuint planeUvboHandle;      // texture coordinate
GLuint planeNboHandle;       // normal 
GLuint planeIboHandle;       // index


// uniform location
GLuint rockProjMatLoc;
GLuint rockViewMatLoc;
GLuint rockModelMatLoc;
GLuint rockAlbedoLoc;      // texture image
GLuint rockPosLoc;         // slime pos vertex shader
GLuint rockPosCompLoc;     // slime pos compute shader
GLuint rockNormalLoc;
GLuint viewPosLoc;

//switch
GLint normalMappingEnabledLoc;


GLuint planeProjMatLoc;
GLuint planeViewMatLoc;
GLuint planeModelMatLoc;
GLuint planeAlbedoLoc;      // texture image
GLuint planePosLoc;         // slime pos vertex shader
GLuint planePosCompLoc;     // slime pos compute shader



void loadmodel(std::string model_path, int id)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(model_path,
		aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	//std::cout << "Model has " << scene->mNumMeshes << " meshes." << std::endl;

	for (unsigned int m = 0; m < scene->mNumMeshes; m++)
	{
		aiMesh* mesh = scene->mMeshes[m];
		indexCount[id] = mesh->mNumFaces * 3;
		vertexCount[id] = mesh->mNumVertices;
		std::cout << "Mesh " << m << " has " << mesh->mNumVertices << " vertices." << std::endl;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			vertexData.push_back(mesh->mVertices[i].x);
			vertexData.push_back(mesh->mVertices[i].y);
			vertexData.push_back(mesh->mVertices[i].z);
			/*std::cout << "Vertex " << i << ": ("
			  << mesh->mVertices[i].x << ", "
			  << mesh->mVertices[i].y << ", "
			  << mesh->mVertices[i].z << ")" << std::endl;*/
			uvData.push_back(mesh->mTextureCoords[0][i].x);
			uvData.push_back(mesh->mTextureCoords[0][i].y);
			uvData.push_back(id);
			/*std::cout << "Texture Coord " << i << ": ("
			 << mesh->mTextureCoords[0][i].x << ", "
			 << mesh->mTextureCoords[0][i].y << ")" << std::endl;*/
			normalData.push_back(mesh->mNormals[i].x);
			normalData.push_back(mesh->mNormals[i].y);
			normalData.push_back(mesh->mNormals[i].z);
			/*std::cout << "normal " << i << ": ("
			 << mesh->mNormals[i].x << ", "
			 << mesh->mNormals[i].y << ", "
			 << mesh->mNormals[i].z << ")" << std::endl;*/
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
				//std::cout << face.mIndices[j] << std::endl;
			}
		}
	}
}

void loadtexture()
{
	// load image
	std::string image_paths[NUM_TEXTURE] = {
	 "assets/bush05.png",
	 "assets/Medieval_Building_LowPoly/Medieval_Building_LowPoly_V1_Albedo_small.png",
	 "assets/Medieval_Building_LowPoly/Medieval_Building_LowPoly_V2_Albedo_small.png",
	 "assets/bush01.png",
	 "assets/grassB_albedo.png",
	};
	// merge the textures to the texture array data
	int width, height, comp;
	for (int i = 0; i < NUM_TEXTURE; ++i) {
		stbi_uc* img = stbi_load(image_paths[i].c_str(), &width, &height, &comp, STBI_rgb_alpha);
		if (img && width == IMG_WIDTH && height == IMG_HEIGHT) {
			memcpy(textureArrayData + i * IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL, img, IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL);
			stbi_image_free(img);
		}
		else {
			std::cerr << "Failed to load image or size mismatch for " << image_paths[i] << std::endl;
			if (img) stbi_image_free(img);
		}
	}

	glGenTextures(1, &textureArrayHandle);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayHandle);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 2, GL_RGBA8, IMG_WIDTH, IMG_HEIGHT, NUM_TEXTURE);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, IMG_WIDTH, IMG_HEIGHT, NUM_TEXTURE, GL_RGBA, GL_UNSIGNED_BYTE, textureArrayData);
	///////////////
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// IMPORTANT !! Use mipmap for the foliage rendering
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}





void loadRockModel()
{
	std::string model_path = "assets/MagicRock/magicRock.obj";
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(model_path,
		aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	//cout << "Model has " << scene->mNumMeshes << " meshes." << endl;

	for (unsigned int m = 0; m < scene->mNumMeshes; m++)
	{
		aiMesh* mesh = scene->mMeshes[m];
		rockIndexCount = mesh->mNumFaces * 3;
		rockVertexCount = mesh->mNumVertices;
		std::cout << "rock Mesh " << m << " has " << mesh->mNumVertices << " vertices." << std::endl;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			rockVertexData.push_back(mesh->mVertices[i].x);
			rockVertexData.push_back(mesh->mVertices[i].y);
			rockVertexData.push_back(mesh->mVertices[i].z);

			/*std::cout << "Vertex " << i << ": ("
			  << mesh->mVertices[i].x << ", "
			  << mesh->mVertices[i].y << ", "
			  << mesh->mVertices[i].z << ")" << std::endl;*/

			rockUvData.push_back(mesh->mTextureCoords[0][i].x);
			rockUvData.push_back(mesh->mTextureCoords[0][i].y);

			/*std::cout << "Texture Coord " << i << ": ("
			 << mesh->mTextureCoords[0][i].x << ", "
			 << mesh->mTextureCoords[0][i].y << ")" << std::endl;*/
			rockNormalData.push_back(mesh->mNormals[i].x);
			rockNormalData.push_back(mesh->mNormals[i].y);
			rockNormalData.push_back(mesh->mNormals[i].z);

			rockTangent.push_back(mesh->mTangents[i].x);
			rockTangent.push_back(mesh->mTangents[i].y);
			rockTangent.push_back(mesh->mTangents[i].z);



			/*std::cout << "tangent " << i << ": ("
			 << mesh->mTangents[i].x << ", "
			 << mesh->mTangents[i].y << ", "
			 << mesh->mTangents[i].z << ")" << std::endl;*/
		}
		//std::cout << mesh->mNumFaces << std::endl;
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {

			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				//std::cout << i << " mFaces "<< ": " << face.mIndices[j] << std::endl;
				rockIndices.push_back(face.mIndices[j]);
			}
		}
	}
}


void loadPlaneModel()
{
	std::string model_path = "assets/airplane.obj";
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(model_path,
		aiProcess_Triangulate);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	//cout << "Model has " << scene->mNumMeshes << " meshes." << endl;

	for (unsigned int m = 0; m < scene->mNumMeshes; m++)
	{
		aiMesh* mesh = scene->mMeshes[m];
		planeIndexCount = mesh->mNumFaces * 3;
		planeVertexCount = mesh->mNumVertices;
		std::cout << "Plane Mesh " << m << " has " << mesh->mNumVertices << " vertices." << std::endl;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			planeVertexData.push_back(mesh->mVertices[i].x);
			planeVertexData.push_back(mesh->mVertices[i].y);
			planeVertexData.push_back(mesh->mVertices[i].z);

			/*std::cout << "Vertex " << i << ": ("
			  << mesh->mVertices[i].x << ", "
			  << mesh->mVertices[i].y << ", "
			  << mesh->mVertices[i].z << ")" << std::endl;*/

			planeUvData.push_back(mesh->mTextureCoords[0][i].x);
			planeUvData.push_back(mesh->mTextureCoords[0][i].y);

			/*std::cout << "Texture Coord " << i << ": ("
			 << mesh->mTextureCoords[0][i].x << ", "
			 << mesh->mTextureCoords[0][i].y << ")" << std::endl;*/
			planeNormalData.push_back(mesh->mNormals[i].x);
			planeNormalData.push_back(mesh->mNormals[i].y);
			planeNormalData.push_back(mesh->mNormals[i].z);

			/*std::cout << "normal " << i << ": ("
			 << mesh->mNormals[i].x << ", "
			 << mesh->mNormals[i].y << ", "
			 << mesh->mNormals[i].z << ")" << std::endl;*/
		}
		//std::cout << mesh->mNumFaces << std::endl;
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {

			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				//std::cout << i << " mFaces "<< ": " << face.mIndices[j] << std::endl;
				planeIndices.push_back(face.mIndices[j]);
			}
		}
	}
}


void loadRockTexture()
{
	glGenTextures(1, &rockTextureHandle);
	glBindTexture(GL_TEXTURE_2D, rockTextureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	rockTextureData = stbi_load("assets/MagicRock/StylMagicRocks_AlbedoTransparency.png", &width, &height, &nrChannels, 0);
	if (rockTextureData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rockTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(rockTextureData);
}

void loadRockNormalTexture()
{
	glGenTextures(1, &rockNormalTextureHandle);
	glBindTexture(GL_TEXTURE_2D, rockNormalTextureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	rockNormalTextureData = stbi_load("assets/MagicRock/StylMagicRocks_NormalOpenGL.png", &width, &height, &nrChannels, 0);
	if (rockNormalTextureData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rockNormalTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(rockNormalTextureData);
}

void loadPlaneTexture()
{
	glGenTextures(1, &planeTextureHandle);
	glBindTexture(GL_TEXTURE_2D, planeTextureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	planeTextureData = stbi_load("assets/Airplane_smooth_DefaultMaterial_BaseMap.jpg", &width, &height, &nrChannels, 0);
	if (planeTextureData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, planeTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(planeTextureData);
}

void initSample() {
	MyPoissonSample* sample0 = MyPoissonSample::fromFile("assets/poissonPoints_1010.ppd2");
	MyPoissonSample* sample1 = MyPoissonSample::fromFile("assets/cityLots_sub_0.ppd2");
	MyPoissonSample* sample2 = MyPoissonSample::fromFile("assets/cityLots_sub_1.ppd2");
	MyPoissonSample* sample3 = MyPoissonSample::fromFile("assets/poissonPoints_2797.ppd2");
	MyPoissonSample* sample4 = MyPoissonSample::fromFile("assets/poissonPoints_621043_after.ppd2");

	NUM_SAMPLE0 = sample0->m_numSample;
	NUM_SAMPLE1 = sample1->m_numSample;
	NUM_SAMPLE2 = sample2->m_numSample;
	NUM_SAMPLE3 = sample3->m_numSample;
	NUM_SAMPLE4 = sample4->m_numSample;

	NUM_TOTAL_INSTANCE = NUM_SAMPLE0+NUM_SAMPLE1+NUM_SAMPLE2+NUM_SAMPLE3+ NUM_SAMPLE4;
	rawInsData = new InstanceProperties[NUM_TOTAL_INSTANCE];

	// query position and rotation
	int k = 0;
	for (int i = 0; i < sample0->m_numSample; i++) {
		glm::vec3 position(sample0->m_positions[i * 3 + 0], 
			               sample0->m_positions[i * 3 + 1], 
			               sample0->m_positions[i * 3 + 2]);
		glm::vec3 rad(sample0->m_radians[i * 3 + 0], 
			          sample0->m_radians[i * 3 + 1], 
			          sample0->m_radians[i * 3 + 2]);
		// calculate rotation matrix
		glm::quat q = glm::quat(rad);
		glm::mat4 rotationMatrix = glm::toMat4(q);
		
		// 存點的位置
		rawInsData[k].position = glm::vec4(position, 1.0);
		k++;
	}
	for (int i = 0; i < sample1->m_numSample; i++) {
		glm::vec3 position(sample1->m_positions[i * 3 + 0],
			sample1->m_positions[i * 3 + 1],
			sample1->m_positions[i * 3 + 2]);
		glm::vec3 rad(sample1->m_radians[i * 3 + 0],
			sample1->m_radians[i * 3 + 1],
			sample1->m_radians[i * 3 + 2]);
		// calculate rotation matrix
		glm::quat q = glm::quat(rad);
		glm::mat4 rotationMatrix = glm::toMat4(q);

		// 存點的位置
		rawInsData[k].position = glm::vec4(position, 1.0);
		k++;
	}
	for (int i = 0; i < sample2->m_numSample; i++) {
		glm::vec3 position(sample2->m_positions[i * 3 + 0],
			sample2->m_positions[i * 3 + 1],
			sample2->m_positions[i * 3 + 2]);
		glm::vec3 rad(sample2->m_radians[i * 3 + 0],
			sample2->m_radians[i * 3 + 1],
			sample2->m_radians[i * 3 + 2]);
		// calculate rotation matrix
		glm::quat q = glm::quat(rad);
		glm::mat4 rotationMatrix = glm::toMat4(q);

		// 存點的位置
		rawInsData[k].position = glm::vec4(position, 1.0);
		k++;
	}
	for (int i = 0; i < sample3->m_numSample; i++) {
		glm::vec3 position(sample3->m_positions[i * 3 + 0],
			sample3->m_positions[i * 3 + 1],
			sample3->m_positions[i * 3 + 2]);
		glm::vec3 rad(sample3->m_radians[i * 3 + 0],
			sample3->m_radians[i * 3 + 1],
			sample3->m_radians[i * 3 + 2]);
		// calculate rotation matrix
		glm::quat q = glm::quat(rad);
		glm::mat4 rotationMatrix = glm::toMat4(q);

		// 存點的位置
		rawInsData[k].position = glm::vec4(position, 1.0);
		k++;
	}
	for (int i = 0; i < sample4->m_numSample; i++) {
		glm::vec3 position(sample4->m_positions[i * 3 + 0],
			sample4->m_positions[i * 3 + 1],
			sample4->m_positions[i * 3 + 2]);
		glm::vec3 rad(sample4->m_radians[i * 3 + 0],
			sample4->m_radians[i * 3 + 1],
			sample4->m_radians[i * 3 + 2]);
		// calculate rotation matrix
		glm::quat q = glm::quat(rad);
		glm::mat4 rotationMatrix = glm::toMat4(q);

		// 存點的位置
		rawInsData[k].position = glm::vec4(position, 1.0);
		k++;
	}
}



void initialize() {
	// prepare a SSBO for storing raw instance data
	GLuint rawInstanceDataBufferHandle;
	glGenBuffers(1, &rawInstanceDataBufferHandle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, rawInstanceDataBufferHandle);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, NUM_TOTAL_INSTANCE * sizeof(InstanceProperties), rawInsData, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, rawInstanceDataBufferHandle);
	// prepare a SSBO for storing VALID instance data
	GLuint validInstanceDataBufferHandle;
	glGenBuffers(1, &validInstanceDataBufferHandle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, validInstanceDataBufferHandle);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, NUM_TOTAL_INSTANCE * sizeof(InstanceProperties), nullptr, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, validInstanceDataBufferHandle);


	drawCommands[0].count = indexCount[0];
	drawCommands[0].instanceCount = 0;
	drawCommands[0].firstIndex = 0;
	drawCommands[0].baseVertex = 0;
	drawCommands[0].baseInstance = 0;

	drawCommands[1].count = indexCount[1];
	drawCommands[1].instanceCount = 0;
	drawCommands[1].firstIndex = indexCount[0];
	drawCommands[1].baseVertex = vertexCount[0];
	drawCommands[1].baseInstance = NUM_SAMPLE0;

	drawCommands[2].count = indexCount[2];
	drawCommands[2].instanceCount = 0;
	drawCommands[2].firstIndex = indexCount[0] + indexCount[1];
	drawCommands[2].baseVertex = vertexCount[0] + vertexCount[1];
	drawCommands[2].baseInstance = NUM_SAMPLE0 + NUM_SAMPLE1;

	drawCommands[3].count = indexCount[3];
	drawCommands[3].instanceCount = 0;
	drawCommands[3].firstIndex = indexCount[0] + indexCount[1] + indexCount[2];
	drawCommands[3].baseVertex = vertexCount[0] + vertexCount[1] + vertexCount[2];
	drawCommands[3].baseInstance = NUM_SAMPLE0 + NUM_SAMPLE1 + NUM_SAMPLE2;

	drawCommands[4].count = indexCount[4];
	drawCommands[4].instanceCount = 0;
	drawCommands[4].firstIndex = indexCount[0] + indexCount[1] + indexCount[2] + indexCount[3];
	drawCommands[4].baseVertex = vertexCount[0] + vertexCount[1] + vertexCount[2] + vertexCount[3];
	drawCommands[4].baseInstance = NUM_SAMPLE0 + NUM_SAMPLE1 + NUM_SAMPLE2 + NUM_SAMPLE3;

	glGenBuffers(1, &cmdBufferHandle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cmdBufferHandle);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(drawCommands), drawCommands, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, cmdBufferHandle);

	// initialize VAO
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);
	// bind the VBO、UVBO、NBO
	glGenBuffers(1, &vboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &uvboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, uvboHandle);
	glBufferData(GL_ARRAY_BUFFER, uvData.size() * sizeof(float), uvData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &nboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, nboHandle);
	glBufferData(GL_ARRAY_BUFFER, normalData.size() * sizeof(float), normalData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &iboHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

	// use SSBO as vertex attribute input
	glBindBuffer(GL_ARRAY_BUFFER, validInstanceDataBufferHandle);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(InstanceProperties), (void*)0);
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);

	glBindVertexArray(0);



	// rock
	// initialize VAO
	glGenVertexArrays(1, &rockVaoHandle);
	glBindVertexArray(rockVaoHandle);
	// bind the VBO、UVBO、NBO
	glGenBuffers(1, &rockVboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, rockVboHandle);
	glBufferData(GL_ARRAY_BUFFER, rockVertexData.size() * sizeof(float), rockVertexData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &rockUvboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, rockUvboHandle);
	glBufferData(GL_ARRAY_BUFFER, rockUvData.size() * sizeof(float), rockUvData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &rockNboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, rockNboHandle);
	glBufferData(GL_ARRAY_BUFFER, rockNormalData.size() * sizeof(float), rockNormalData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	//tangent
	glGenBuffers(1, &rockTboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, rockTboHandle);
	glBufferData(GL_ARRAY_BUFFER, rockTangent.size() * sizeof(float), rockTangent.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(6);
	std::cout << rockTangent.data() << std::endl;

	glGenBuffers(1, &rockIboHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rockIboHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, rockIndices.size() * sizeof(float), rockIndices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);


	// plane
	// initialize VAO
	glGenVertexArrays(1, &planeVaoHandle);
	glBindVertexArray(planeVaoHandle);
	// bind the VBO、UVBO、NBO
	glGenBuffers(1, &planeVboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, planeVboHandle);
	glBufferData(GL_ARRAY_BUFFER, planeVertexData.size() * sizeof(float), planeVertexData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &planeUvboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, planeUvboHandle);
	glBufferData(GL_ARRAY_BUFFER, planeUvData.size() * sizeof(float), planeUvData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &planeNboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, planeNboHandle);
	glBufferData(GL_ARRAY_BUFFER, planeNormalData.size() * sizeof(float), planeNormalData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &planeIboHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeIboHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeIndices.size() * sizeof(float), planeIndices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);

}

void setUniformVariables() {
	viewProjMatLoc = glGetUniformLocation(myCompShaderPointer->ID, "viewProjMat");
	viewMatLoc = glGetUniformLocation(myShaderPointer->ID, "viewMat");
	projMatLoc = glGetUniformLocation(myShaderPointer->ID, "projMat");
	modelMatLoc = glGetUniformLocation(myShaderPointer->ID, "modelMat");
	albedoLoc = glGetUniformLocation(myShaderPointer->ID, "albedoTextureArray");

	rockViewMatLoc = glGetUniformLocation(rockShaderPointer->ID, "viewMat");
	rockProjMatLoc = glGetUniformLocation(rockShaderPointer->ID, "projMat");
	rockModelMatLoc = glGetUniformLocation(rockShaderPointer->ID, "modelMat");
	rockAlbedoLoc = glGetUniformLocation(rockShaderPointer->ID, "albedoTexture");
	rockNormalLoc = glGetUniformLocation(rockShaderPointer->ID, "NormalTexture");
	rockPosLoc = glGetUniformLocation(rockShaderPointer->ID, "v_worldPosOffset");
	viewPosLoc = glGetUniformLocation(rockShaderPointer->ID, "viewPos");

	normalMappingEnabledLoc = glGetUniformLocation(rockShaderPointer->ID, "u_NormalMappingEnabled");

	planeViewMatLoc = glGetUniformLocation(planeShaderPointer->ID, "viewMat");
	planeProjMatLoc = glGetUniformLocation(planeShaderPointer->ID, "projMat");
	planeModelMatLoc = glGetUniformLocation(planeShaderPointer->ID, "modelMat");
	planeAlbedoLoc = glGetUniformLocation(planeShaderPointer->ID, "albedoTexture");
	planePosLoc = glGetUniformLocation(planeShaderPointer->ID, "v_worldPosOffset");
	
}
void myInit() {
	std::cout << "testing\n";
	
	loadmodel("assets/bush05_lod2.obj", 0);
	loadmodel("assets/Medieval_Building_LowPoly/medieval_building_lowpoly_1.obj", 1);
	loadmodel("assets/Medieval_Building_LowPoly/medieval_building_lowpoly_2.obj", 2);
	loadmodel("assets/bush01_lod2.obj", 3);
	loadmodel("assets/grassB.obj", 4);

	loadtexture();

	loadRockModel();
	loadRockTexture();
	loadRockNormalTexture();

	loadPlaneModel();
	loadPlaneTexture();
	const std::string vsFile = "src/shader/myVertexShader.glsl";
	const std::string fsFile = "src/shader/myFragmentShader.glsl";
	const std::string csFile = "src/shader/myComputeShader.glsl";
	const std::string rcsFile = "src/shader/myResetComputeShader.glsl";
	const std::string rockvsFile = "src/shader/rockVertexShader.glsl";
	const std::string rockfsFile = "src/shader/rockFragmentShader.glsl";
	const std::string planevsFile = "src/shader/planeVertexShader.glsl";
	const std::string planefsFile = "src/shader/planeFragmentShader.glsl";

	myShaderPointer = new MyShader(vsFile.c_str(), fsFile.c_str());
	rockShaderPointer = new MyShader(rockvsFile.c_str(), rockfsFile.c_str());
	planeShaderPointer = new MyShader(planevsFile.c_str(), planefsFile.c_str());
	myCompShaderPointer = new MyComputeShader(csFile.c_str());
	myResetCompShaderPointer = new MyComputeShader(rcsFile.c_str());
	initSample();
	initialize();
}
void myComputeRender(glm::mat4 playerProjectionMatrix, glm::mat4 playerViewMatrix) {
	
	// reset shader
	myResetCompShaderPointer->use();
	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// compute shader
	myCompShaderPointer->use();
	glUniform1i(glGetUniformLocation(myCompShaderPointer->ID, "numMaxInstance"), NUM_TOTAL_INSTANCE);
	const glm::mat4  viewProj = playerProjectionMatrix * playerViewMatrix;
	GLuint viewProjMatLoc = glGetUniformLocation(myCompShaderPointer->ID, "viewProjMat");
	glUniformMatrix4fv(viewProjMatLoc, 1, false, glm::value_ptr(viewProj));
	glDispatchCompute((NUM_TOTAL_INSTANCE / 1024) + 1, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


}


void myGodRender(glm::mat4 godProjectionMatrix, glm::mat4 godViewMatrix, const INANOA::MyCameraManager* m_myCameraManager, const MyImGuiPanel* m_imguiPanel) {
	setUniformVariables();


	const glm::mat4 airplaneModelMat = m_myCameraManager->airplaneModelMatrix();

	glm::mat4 model_matrix = glm::mat4(1.0);
	glm::vec4 positionVec4 = glm::vec4(25.92, 18.27, 11.75, 1.0);
	glm::vec4 planepositionVec4 = glm::vec4(0.0, 0.0, 0.0, 1.0);

	// myShader
	myShaderPointer->use();
	glUniformMatrix4fv(projMatLoc, 1, false, glm::value_ptr(godProjectionMatrix));
	glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(godViewMatrix));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayHandle);
	glUniform1i(albedoLoc, 0);

	// bind VAO
	glBindVertexArray(vaoHandle);
	// bind draw-indirect-buffer
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmdBufferHandle);
	// render
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, 5, sizeof(DrawElementsIndirectCommand));

	//rock
	rockShaderPointer->use();
	glUniformMatrix4fv(rockProjMatLoc, 1, GL_FALSE, glm::value_ptr(godProjectionMatrix));
	glUniformMatrix4fv(rockViewMatLoc, 1, GL_FALSE, glm::value_ptr(godViewMatrix));
	glUniformMatrix4fv(rockModelMatLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
	glUniform4fv(rockPosLoc, 1, glm::value_ptr(positionVec4));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, rockTextureHandle);
	glUniform1i(rockAlbedoLoc, 3);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, rockNormalTextureHandle);//Normal mapping
	glUniform1i(rockNormalLoc, 5);
	glBindVertexArray(rockVaoHandle);

	glUniform3fv(viewPosLoc, 1, glm::value_ptr(m_myCameraManager->airplanePosition()));


	//switch
	bool normalMappingEnabled = m_imguiPanel->isNormalMappingEnabled();
	glUseProgram(rockShaderPointer->ID); 
	glUniform1i(normalMappingEnabledLoc, normalMappingEnabled);


	glDrawElements(GL_TRIANGLES, rockVertexCount, GL_UNSIGNED_INT, 0);

	//plane
	planeShaderPointer->use();
	glUniformMatrix4fv(planeProjMatLoc, 1, GL_FALSE, glm::value_ptr(godProjectionMatrix));
	glUniformMatrix4fv(planeViewMatLoc, 1, GL_FALSE, glm::value_ptr(godViewMatrix));
	glUniformMatrix4fv(planeModelMatLoc, 1, GL_FALSE, glm::value_ptr(airplaneModelMat));
	glUniform4fv(planePosLoc, 1, glm::value_ptr(planepositionVec4));
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, planeTextureHandle);
	glUniform1i(planeAlbedoLoc, 4);
	glBindVertexArray(planeVaoHandle);

	glDrawElements(GL_TRIANGLES, planeVertexCount, GL_UNSIGNED_INT, 0);

}
void myPlayerRender(glm::mat4 playerProjectionMatrix, glm::mat4 playerViewMatrix, const INANOA::MyCameraManager* m_myCameraManager, const MyImGuiPanel* m_imguiPanel) {
	setUniformVariables();


	const glm::mat4 airplaneModelMat = m_myCameraManager->airplaneModelMatrix();

	glm::mat4 model_matrix = glm::mat4(1.0);
	glm::vec4 positionVec4 = glm::vec4(25.92, 18.27, 11.75, 1.0);
	glm::vec4 planepositionVec4 = glm::vec4(0.0, 0.0, 0.0, 1.0);
	// myShader
	myShaderPointer->use();
	glUniformMatrix4fv(projMatLoc, 1, false, glm::value_ptr(playerProjectionMatrix));
	glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(playerViewMatrix));
	
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayHandle);
	glUniform1i(albedoLoc, 0);

	// bind VAO
	glBindVertexArray(vaoHandle);
	// bind draw-indirect-buffer
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmdBufferHandle);
	// render
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, 5, sizeof(DrawElementsIndirectCommand));

	//rock
	rockShaderPointer->use();
	glUniformMatrix4fv(rockProjMatLoc, 1, GL_FALSE, glm::value_ptr(playerProjectionMatrix));
	glUniformMatrix4fv(rockViewMatLoc, 1, GL_FALSE, glm::value_ptr(playerViewMatrix));
	glUniformMatrix4fv(rockModelMatLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
	glUniform4fv(rockPosLoc, 1, glm::value_ptr(positionVec4));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, rockTextureHandle);
	glUniform1i(rockAlbedoLoc, 3);
	glBindVertexArray(rockVaoHandle);

	glDrawElements(GL_TRIANGLES, rockVertexCount, GL_UNSIGNED_INT, 0);

	//plane
	planeShaderPointer->use();
	glUniformMatrix4fv(planeProjMatLoc, 1, GL_FALSE, glm::value_ptr(playerProjectionMatrix));
	glUniformMatrix4fv(planeViewMatLoc, 1, GL_FALSE, glm::value_ptr(playerViewMatrix));
	glUniformMatrix4fv(planeModelMatLoc, 1, GL_FALSE, glm::value_ptr(airplaneModelMat));
	glUniform4fv(planePosLoc, 1, glm::value_ptr(planepositionVec4));
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, planeTextureHandle);
	glUniform1i(planeAlbedoLoc, 4);
	glBindVertexArray(planeVaoHandle);

	glDrawElements(GL_TRIANGLES, planeVertexCount, GL_UNSIGNED_INT, 0);
}

