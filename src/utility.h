#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include "lodepng.h"

// add leading zeros to a number
// ex ==> leadingZeros(5,3) = "005"
// ex ==> leadingZeros(123,5) = "00123"
std::string leadingZeros(int number,int zeros)
{
	char Buffer[100];
	int RespCode;
	RespCode = snprintf(Buffer, 100, "%0*d", zeros, number);
	return std::string(Buffer);
}

// convert patch ID to vec2 (UV space)
// id range is [1;16]
glm::vec2 idToVec2(int id)
{
	id =id-1;
	int y = id/4;
	int x = id%4;
	float step=0.25;
	return glm::vec2(step*(x+0.5f),step*(y+0.5f));
}

// split txt, puts result into strs
// delimiter is ch
static size_t str_split(const std::string &txt, std::vector<std::string> &strs, char ch)
	{
		size_t pos = txt.find(ch);
		size_t initialPos = 0;
		strs.clear();

		// Decompose statement
		while (pos != std::string::npos)
		{
			strs.push_back(txt.substr(initialPos, pos - initialPos));
			initialPos = pos + 1;

			pos = txt.find(ch, initialPos);
		}

		// Add the last one
		strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

		return strs.size();
	}

auto time()
{
 	return std::chrono::high_resolution_clock::now();
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

class Timer {
	private:
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1> >;
	std::chrono::time_point<clock_t> m_beg;

public:
	Timer()
	{
		reset();
	}
	void reset()
	{
		m_beg = clock_t::now();
	}
	double elapsed()
	{
		return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
	}
};