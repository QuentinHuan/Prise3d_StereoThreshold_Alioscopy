#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include "lodepng.h"
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>

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

// returns "X=[v.x] Y=[v.y]
std::string vec2ToString(glm::vec2 v)
{
	return "X="+std::to_string(v.x)+" Y="+std::to_string(v.y);
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

/*
 return next stimuli values (in [1,100]^16) for scene sceneName
 values are coma separated
 use the results files in ./data to compute the new value 
 */
std::vector<int> next_stimulus_MLE(std::string sceneName)
{
	std::vector<int> result = std::vector<int>();
	//FGenericPlatformMisc::OsExecute(TEXT("python"), TEXT("python E : /git/Prise3D_StereoThreshold/DataProcessing/simulation_adaptative_sampling.py '250; 1'"));
	std::string directory = "../";// root directory
	std::string cmd = "python3 "+ directory + "script/ComputeNewStimulusSet.py " + sceneName + " && exit";

	const int MAX_BUFFER = 2048;
	char buffer[MAX_BUFFER];

	// execute python script in a terminal
	
	FILE* in = popen(cmd.c_str(), "r");

	// pipe the terminal output into "std::string out"
	std::string out= std::string(cmd);
	if (in) {
		while (!feof(in))
		{
			if (fgets(buffer, MAX_BUFFER, in) != NULL)
			{
				out = std::string(buffer);
			}
		}
		pclose(in);
	}
	std::vector<std::string> split = std::vector<std::string>();
    str_split(out,split,',');

	for (int i = 0; i < split.size(); i++)
	{
		result.push_back(stoi(split.at(i)));
	}

	return result;
}


// print experiment result into log files (located in .\P3D\WindowsNoEditor\P3d_Expe1\Saved\Logs)
// 
// format :
// "p3d:p3d_[scene]_[RightOrLeft];[time];[gazePos];[noisePatchPos];[noiseValue];[detect]"
void log(int showNoiseLeft, int showNoiseRight, std::string sceneName, float time, glm::vec2 gazePos, glm::vec2 noisePatchPos, int noiseValue, int detect)
{
	std::ofstream outfile;
	outfile.open("../log/p3d.log", std::ios_base::app); // append instead of overwrite

	std::string result = "p3d:";
	result.append(sceneName); // p3d_bathroom

	std::string suffix = "";// _right , _left or none
	if(showNoiseLeft==1 && showNoiseRight==0) suffix = "_left";
	if (showNoiseLeft == 0 && showNoiseRight == 1) suffix = "_right";
	result.append(suffix); 

	result.append(";");
	result.append(std::to_string(time));

	result.append(";");
	result.append(vec2ToString(gazePos));

	result.append(";");
	result.append(vec2ToString(noisePatchPos));

	result.append(";");
	result.append(std::to_string(noiseValue));

	result.append(";");
	result.append(std::to_string(detect));
	// format :
	// "p3d:p3d_bathroom_right;[time];[gazePos];[noisePatchPos];[noiseValue];[detect]"
	outfile << result << std::endl; 
	return;
}


std::string saveExperiment()
{
	std::string directory = "../";
	std::string cmd = "python3 " + directory + "script/ExperimentAnalysis.py" + " && exit";

	FILE* in = popen(cmd.c_str(), "r");

	const int MAX_BUFFER = 2048;
	char buffer[MAX_BUFFER];

	// pipe the terminal output into "std::string out"
	std::string out = std::string(cmd);
	if (in) {
		while (!feof(in))
		{
			if (fgets(buffer, MAX_BUFFER, in) != NULL)
			{
				out = std::string(buffer);
			}
		}
		pclose(in);
	}

	return out;
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