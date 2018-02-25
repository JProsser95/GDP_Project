// Fill out your copyright notice in the Description page of Project Settings.

#include "PlanePointManager.h"
#include "ToyPlane.h"
#include "Macros.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"


// Sets default values
APlanePointManager::APlanePointManager()
	:ToyPlane(nullptr), BoostIncrement(3.125f), VisibleRings(10), RingSmallScale(0.25f)
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

	for (int i = 1; i < GetVisibleActors(); ++i)
	{
		Actors[i]->SetActorHiddenInGame(false);
		Actors[i]->SetActorRelativeScale3D(FVector(RingSmallScale));
	}
	for (int i = VisibleRings; i < Actors.Num(); ++i)
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
		int i(GetVisibleActors() - 1);
		if (i >= 0)
		{
			Actors[i]->SetActorHiddenInGame(false);
			Actors[i]->SetActorRelativeScale3D(FVector(RingSmallScale));
		}
	}
	if (!Actors.Num())
		AllPointsCollected();
	else
	{
		float scale = Actors[0]->GetActorScale3D().X;
		if (scale + DeltaTime <= 1.0f)
			scale += DeltaTime;
		else
			scale = 1.0f;
		Actors[0]->SetActorRelativeScale3D(FVector(scale));
	}

}

int APlanePointManager::GetVisibleActors()
{
	if (Actors.Num() >= VisibleRings)
		return VisibleRings;
	return Actors.Num();
}

void APlanePointManager::AllPointsCollected()
{
	OUTPUT_FSTRING("All \"rings\" have been collected!");
}