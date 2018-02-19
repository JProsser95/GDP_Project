// Fill out your copyright notice in the Description page of Project Settings.

#include "PlanePointManager.h"
#include "ToyPlane.h"
#include "Macros.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"


// Sets default values
APlanePointManager::APlanePointManager()
	:ToyPlane(nullptr), BoostIncrement(50.0f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlanePointManager::BeginPlay()
{
	Super::BeginPlay();
	if (!ToyPlane)
	{
		UE_LOG(LogTemp, Error, TEXT("ToyPlane has not been setup in PlanePointManager!"));
	}

	for (int i = 3; i < Actors.Num(); ++i)
	{
		Actors[i]->SetActorHiddenInGame(true);
	}
}

// Called every frame
void APlanePointManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Actors.Num())
		return;

	
	if (Actors[0]->IsOverlappingActor(ToyPlane))
	{
		ToyPlane->UpdateCurrentBoost(BoostIncrement);

		GetWorld()->DestroyActor(Actors[0]);
		Actors.RemoveAt(0);
		for (int i = 0; i < GetVisibleActors(); ++i)
		{
			Actors[i]->SetActorHiddenInGame(false);
		}
		if (!Actors.Num())
			AllPointsCollected();
		return;
	}

}

int APlanePointManager::GetVisibleActors()
{
	if (Actors.Num() >= 3)
		return 3;
	return Actors.Num();
}

void APlanePointManager::AllPointsCollected()
{
	OUTPUT_FSTRING("All \"rings\" have been collected!");
}