// Fill out your copyright notice in the Description page of Project Settings.

#include "TrainPuzzle.h"


// Sets default values
ATrainPuzzle::ATrainPuzzle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATrainPuzzle::BeginPlay()
{
	Super::BeginPlay();
	
	TrainPuzzleStates.SetNum(Triggers.Num());
}

// Called every frame
void ATrainPuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckAndUpdateTriggers();

	if (ToyTrain->TrainPuzzleFailed())
	{
		ResetToLastCheckpoint();
	}


}

void ATrainPuzzle::CheckAndUpdateTriggers()
{
	for (int i = 0; i < Triggers.Num(); ++i)
	{
		if (!ToyTrain->OnFailureTrainLine() && Triggers[i] && Triggers[i]->IsOverlappingActor(ToyCar))
		{
			Triggers[i]->Destroy();
			Triggers[i] = nullptr;

			TrainPuzzleStates[i].CarPosition = ToyCar->GetActorLocation();
			TrainPuzzleStates[i].CarRotation = ToyCar->GetActorRotation();
			TrainPuzzleStates[i].TrainSplineCounter = ToyTrain->GetSplineCounter();
			TrainPuzzleStates[i].TrainState = ToyTrain->GetTrainState();
		}
	}
}

void ATrainPuzzle::ResetToLastCheckpoint()
{
	for (int i = Triggers.Num() - 1; i >= 0; --i)
	{
		if (!Triggers[i])
		{
			ToyCar->SetActorRelativeLocation(TrainPuzzleStates[i].CarPosition, false, NULL, ETeleportType::TeleportPhysics);
			ToyCar->SetActorRotation(FQuat(TrainPuzzleStates[i].CarRotation), ETeleportType::TeleportPhysics);
			ToyTrain->SetSplineCounter(TrainPuzzleStates[i].TrainSplineCounter);
			ToyTrain->SetTrainState(TrainPuzzleStates[i].TrainState);
			return; // We're done, return out
		}
	}
}

