// Fill out your copyright notice in the Description page of Project Settings.


#include "P3dComponent.h"

// Sets default values for this component's properties
UP3dComponent::UP3dComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UP3dComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UP3dComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// find and return a string containing text file located in .\P3D\WindowsNoEditor\P3d_Expe1\Content\data
FString UP3dComponent::ReadFile(FString filename)
{
	//Read file ini [project]/Content/Data/ 
		//you can change with other location
	FString directory = FPaths::ProjectContentDir();
	FString result;
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
	FString myFile = directory + "data/" + filename;
	FFileHelper::LoadFileToString(result, *myFile);
	return result;
}

// return a random FVector2D on a grid
//
// exemple with cellSize = 1/4 :
/*
|x|x|x|x|
|x|x|x|x|
|x|x|x|x|
|x|x|x|x|
*/
FVector2D UP3dComponent::random2d(float CellSize)
{
	FVector2D result = FVector2D();
	if (CellSize != 0)
	{
		float inv = 1.0f / CellSize;
		int32 range = FMath::TruncToInt(inv);
		int32 x = FMath::RandRange(1, range);
		int32 y = FMath::RandRange(1, range);

		result = FVector2D(float(x), float(y));

		FVector2D offset = FVector2D(CellSize / 2.0f);
		result = (result / inv) - (offset);
		return result;
	}
	else
	{
		return result;
	}
}
// print experiment result into log files (located in .\P3D\WindowsNoEditor\P3d_Expe1\Saved\Logs)
// 
// format :
// "p3d:p3d_[scene]_[RightOrLeft];[time];[gazePos];[noisePatchPos];[noiseValue];[detect]"
FString UP3dComponent::log(int32 showNoiseLeft, int32 showNoiseRight, FString sceneName, float time, FVector2D gazePos, FVector2D noisePatchPos, int32 noiseValue, int32 detect)
{
	FString result = FString(TEXT("p3d:"));
	result.Append(sceneName); // p3d_bathroom

	FString suffix = FString(TEXT(""));// _right , _left or none
	if(showNoiseLeft==1 && showNoiseRight==0) suffix = FString(TEXT("_left"));
	if (showNoiseLeft == 0 && showNoiseRight == 1) suffix = FString(TEXT("_right"));
	result.Append(suffix); 

	result.Append(TEXT(";"));
	result.Append(FString::SanitizeFloat(time));

	result.Append(TEXT(";"));
	result.Append(gazePos.ToString());

	result.Append(TEXT(";"));
	result.Append(noisePatchPos.ToString());

	result.Append(TEXT(";"));
	result.Append(FString::FromInt(noiseValue));

	result.Append(TEXT(";"));
	result.Append(FString::FromInt(detect));
	// format :
	// "p3d:p3d_bathroom_right;[time];[gazePos];[noisePatchPos];[noiseValue];[detect]"
	return result;
}
// return image path, given ImageFolder Path, sceneName, right or left eye, and noiseLevel
//
// format:
// [ImageFolderPath]\[sceneName]-[LeftOrRight]\[sceneName]-[LeftOrRight]_1_[noiseLevel].png
FString UP3dComponent::getImagePath(FString ImageFolderPath, FString sceneName, FString LeftOrRight, int32 noiseLevel)
{
	// "ImageFolderPath\"
	FString path = ImageFolderPath;

	// "[sceneName]-[LeftOrRight]\"
	FString folder = FString(TEXT(""));
	folder.Append(sceneName);
	folder.Append(TEXT("-"));
	folder.Append(LeftOrRight);
	folder.Append(TEXT("\\"));

	path.Append(folder);

	// [sceneName]-[LeftOrRight]_[00000].png
	FString file = FString(TEXT(""));
	file.Append(sceneName);
	file.Append(TEXT("-"));
	file.Append(LeftOrRight);
	file.Append(TEXT("_"));
	file.Append(leadingZeros(noiseLevel,5));
	file.Append(TEXT(".png"));

	path.Append(file);

	return path;
}

void UP3dComponent::saveStringToFile(FString string)
{
	FString l, r;
	string.Split("=",&l,&r);
	FString directory = FPaths::ProjectContentDir();
	FString myFile = directory + "data/" + l + ".txt";
	FFileHelper::SaveStringToFile(string,*myFile);
}

FString UP3dComponent::loadStringFromFile(FString string)
{
	return ReadFile(string + ".txt");
}

FString UP3dComponent::leadingZeros(int32 number,int32 zeros)
{
	char Buffer[100];
	int RespCode;
	RespCode = snprintf(Buffer, 100, "%0*d", zeros, number);
	return FString(ANSI_TO_TCHAR(Buffer));
}

