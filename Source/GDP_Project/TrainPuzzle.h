// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerBox.h"

#include "ToyCar.h"
#include "ToyTrain.h"

#include "TrainPuzzle.generated.h"

struct TrainPuzzleState
{
	FVector CarPosition;
	FRotator CarRotation;

	int TrainSplineCounter;
	TRAIN_STATES TrainState;
};

UCLASS()
class GDP_PROJECT_API ATrainPuzzle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrainPuzzle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CheckAndUpdateTriggers();

	const float m_fFailDelay = 2.0f;

	float m_fFailCounter;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ResetToLastCheckpoint();

	TArray<TrainPuzzleState> TrainPuzzleStates;
	
	UPROPERTY(EditAnywhere)
	TArray<ATriggerBox*> Triggers;

	UPROPERTY(EditAnywhere)
	AToyCar* ToyCar;

	UPROPERTY(EditAnywhere)
	AToyTrain* ToyTrain;
};
