// Fill out your copyright notice in the Description page of Project Settings.

#include "PointToPointManager.h"
#include "Macros.h"


// Sets default values
APointToPointManager::APointToPointManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APointToPointManager::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 3; i < Actors.Num(); ++i)
	{
		Actors[i]->SetActorHiddenInGame(true);
	}
}

// Called every frame
void APointToPointManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Actors.Num())
		return;

	TArray<UPrimitiveComponent*> components;
	
	Actors[0]->GetOverlappingComponents(components);

	for (UPrimitiveComponent* pComponent : components)
	{
		if (pComponent->GetCollisionObjectType() == ECollisionChannel::ECC_Vehicle)
		{
			GetWorld()->DestroyActor(Actors[0]);
			Actors.RemoveAt(0);
			for (int i = 0; i < GetVisibleActors(); ++i)
			{
				Actors[i]->SetActorHiddenInGame(false);
			}
			return;
		}
	}

}

int APointToPointManager::GetVisibleActors()
{
	if (Actors.Num() >= 3)
		return 3;
	return Actors.Num();
}

bool APointToPointManager::AllPointsCollected()
{
	if (!Actors.Num())
		return true;

	return false;
}