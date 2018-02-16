// Fill out your copyright notice in the Description page of Project Settings.

#include "TrackSwappingManager.h"


// Sets default values
ATrackSwappingManager::ATrackSwappingManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATrackSwappingManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrackSwappingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int ATrackSwappingManager::GetNearestSwapper(AActor* ToyTrain)
{
	for (int i = 0; i < Swappers.Num(); ++i)
	{
		if (Swappers[i]->IsOverlappingActor(ToyTrain))
		{
			return i;
		}
	}

	return -1;
}
