#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "lodepng.h"
struct Ray
{
	glm::vec3 Direction;
	glm::vec3 Origin;
};

struct Triangle
{
	glm::vec3 A, B, C;
};

struct textureDataBuffer
{
  std::vector<unsigned char> bitmapData;
  unsigned width, height;
};
//-----------------------------------------------------------------------------
// Purpose: add vectex to vertdata array
// format: | x y z | u v |
//-----------------------------------------------------------------------------
void AddVertex(float fl0, float fl1, float fl2, float fl3, float fl4, std::vector<float> &vertdata)
{
	vertdata.push_back(fl0);
	vertdata.push_back(fl1);
	vertdata.push_back(fl2);
	vertdata.push_back(fl3);
	vertdata.push_back(fl4);
}

//######################################
//      		 cube
//######################################
void AddCubeToScene(glm::mat4 T, std::vector<float> &vertdata)
{
	// glm::mat44 T( outermat.data() );

	glm::vec4 A = T * glm::vec4(0, 0, 0, 1);
	glm::vec4 B = T * glm::vec4(1, 0, 0, 1);
	glm::vec4 C = T * glm::vec4(1, 1, 0, 1);
	glm::vec4 D = T * glm::vec4(0, 1, 0, 1);
	glm::vec4 E = T * glm::vec4(0, 0, 1, 1);
	glm::vec4 F = T * glm::vec4(1, 0, 1, 1);
	glm::vec4 G = T * glm::vec4(1, 1, 1, 1);
	glm::vec4 H = T * glm::vec4(0, 1, 1, 1);

	// triangles instead of quads
	AddVertex(E.x, E.y, E.z, 0, 1, vertdata); //Front
	AddVertex(F.x, F.y, F.z, 1, 1, vertdata);
	AddVertex(G.x, G.y, G.z, 1, 0, vertdata);
	AddVertex(G.x, G.y, G.z, 1, 0, vertdata);
	AddVertex(H.x, H.y, H.z, 0, 0, vertdata);
	AddVertex(E.x, E.y, E.z, 0, 1, vertdata);

	AddVertex(B.x, B.y, B.z, 0, 1, vertdata); //Back
	AddVertex(A.x, A.y, A.z, 1, 1, vertdata);
	AddVertex(D.x, D.y, D.z, 1, 0, vertdata);
	AddVertex(D.x, D.y, D.z, 1, 0, vertdata);
	AddVertex(C.x, C.y, C.z, 0, 0, vertdata);
	AddVertex(B.x, B.y, B.z, 0, 1, vertdata);

	AddVertex(H.x, H.y, H.z, 0, 1, vertdata); //Top
	AddVertex(G.x, G.y, G.z, 1, 1, vertdata);
	AddVertex(C.x, C.y, C.z, 1, 0, vertdata);
	AddVertex(C.x, C.y, C.z, 1, 0, vertdata);
	AddVertex(D.x, D.y, D.z, 0, 0, vertdata);
	AddVertex(H.x, H.y, H.z, 0, 1, vertdata);

	AddVertex(A.x, A.y, A.z, 0, 1, vertdata); //Bottom
	AddVertex(B.x, B.y, B.z, 1, 1, vertdata);
	AddVertex(F.x, F.y, F.z, 1, 0, vertdata);
	AddVertex(F.x, F.y, F.z, 1, 0, vertdata);
	AddVertex(E.x, E.y, E.z, 0, 0, vertdata);
	AddVertex(A.x, A.y, A.z, 0, 1, vertdata);

	AddVertex(A.x, A.y, A.z, 0, 1, vertdata); //Left
	AddVertex(E.x, E.y, E.z, 1, 1, vertdata);
	AddVertex(H.x, H.y, H.z, 1, 0, vertdata);
	AddVertex(H.x, H.y, H.z, 1, 0, vertdata);
	AddVertex(D.x, D.y, D.z, 0, 0, vertdata);
	AddVertex(A.x, A.y, A.z, 0, 1, vertdata);

	AddVertex(F.x, F.y, F.z, 0, 1, vertdata); //Right
	AddVertex(B.x, B.y, B.z, 1, 1, vertdata);
	AddVertex(C.x, C.y, C.z, 1, 0, vertdata);
	AddVertex(C.x, C.y, C.z, 1, 0, vertdata);
	AddVertex(G.x, G.y, G.z, 0, 0, vertdata);
	AddVertex(F.x, F.y, F.z, 0, 1, vertdata);
}
//######################################
//      		 Plane
//######################################
void AddPlaneToScene(glm::mat4 T, std::vector<float> &vertdata)
{
	// glm::mat4 T( outermat.data() );
	glm::vec4 E = T * glm::vec4(0, 0, 1, 1);
	glm::vec4 F = T * glm::vec4(1, 0, 1, 1);
	glm::vec4 G = T * glm::vec4(1, 1, 1, 1);
	glm::vec4 H = T * glm::vec4(0, 1, 1, 1);

	// triangles instead of quads
	AddVertex(E.x, E.y, E.z, 0, 1, vertdata); //Front
	AddVertex(F.x, F.y, F.z, 1, 1, vertdata);
	AddVertex(G.x, G.y, G.z, 1, 0, vertdata);
	AddVertex(G.x, G.y, G.z, 1, 0, vertdata);
	AddVertex(H.x, H.y, H.z, 0, 0, vertdata);
	AddVertex(E.x, E.y, E.z, 0, 1, vertdata);
}

//######################################
//      		 DEBUG
//######################################
void printMatrix4(glm::mat4 M)
{

	for (size_t i = 0; i < 4; i++)
	{
		printf("| %f %f %f %f |\n", M[i][0], M[i][1], M[i][2], M[i][3]);
	}
	printf("\n");
}

void printVec3(glm::vec3 V)
{
	printf("|%f|%f|%f|\n", V.x, V.y, V.z);
}
void printVec2(glm::vec2 V)
{
	printf("|%f|%f|\n", V.x, V.y);
}
void printTriangle(Triangle T)
{
	printf("A :");
	printVec3(T.A);
	printf("B :");
	printVec3(T.B);
	printf("C :");
	printVec3(T.C);
}

glm::vec3 IntersectPlane(Ray ray, Triangle T)
{
	float INTERSECT_EPSILON = 0.0001f;
	glm::vec3 e0 = T.B - T.A;
	glm::vec3 e1 = T.A - T.C;
	glm::vec3 triangleNormal = glm::cross(e1, e0);

	float invDot = (1.0f / glm::dot(triangleNormal, ray.Direction));
	glm::vec3 e2 = (T.A - ray.Origin) * invDot;
	glm::vec3 i = glm::cross(ray.Direction, e2);

	float k = glm::dot(triangleNormal, e2); //ray parametric eq

	return ray.Origin + ray.Direction * k;
}

textureDataBuffer createBitmapData(std::string path)
{
	// load and generate the texture
	textureDataBuffer out;
	unsigned width, height, nrChannels;
	std::vector<unsigned char> data;

	unsigned nError = lodepng::decode(data, width, height, path);

	if (nError != 0)
	{
		std::cout << "Failed to load texture" << std::endl;
		return out;
	}
	else
	{
		out.bitmapData = data;
		out.width = width;
		out.height = height;
		return out;
	}
}
