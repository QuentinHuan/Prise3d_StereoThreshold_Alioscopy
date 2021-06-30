// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "P3dComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class P3D_EXPE1_API UP3dComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UP3dComponent();

	struct parameters
	{
		float PatchSize;
		float F;
		float FS;
		float ExperimentTime;
		float aimRadius;
		float ShowNoiseLeft, ShowNoiseRight;
		float RadiusMin, RadiusMax;

		int32 RefValue;
		int32 maxNoiseValue;

		FString ImagePath;

		bool PolarNoiseDistrib;
		bool noiseFollowGazeDir;
		bool invertNoise;
	};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	FString leadingZeros(int32 number, int32 zeros);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable) FString ReadFile(FString filename);
	UFUNCTION(BlueprintCallable) FVector2D random2d(float CellSize);
	UFUNCTION(BlueprintCallable) FString log(int32 showNoiseLeft, int32 showNoiseRight, FString sceneName, float time, FVector2D noisePatchPos, FVector2D gazePos, int32 noiseValue, int32 detect);
	UFUNCTION(BlueprintCallable) FString getImagePath(FString ImageFolderPath, FString sceneName, FString LeftOrRight, int32 noiseLevel);
	UFUNCTION(BlueprintCallable) void saveStringToFile(FString string);
	UFUNCTION(BlueprintCallable) FString loadStringFromFile(FString string);
};
