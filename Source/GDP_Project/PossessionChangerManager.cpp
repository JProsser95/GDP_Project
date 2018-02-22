// Fill out your copyright notice in the Description page of Project Settings.

#include "PossessionChangerManager.h"
#include "Engine/World.h"

// Sets default values
APossessionChangerManager::APossessionChangerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentVehicle = POSSESSABLE_VEHICLES::Car;
}

// Called when the game starts or when spawned
void APossessionChangerManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APossessionChangerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APossessionChangerManager::CheckPossessionPads()
{
	for (int i = 0; i < PossessionChangers.Num(); ++i)
	{
		if (PossessionChangers[i]->VehicleToChangeFrom == CurrentVehicle)
		{
			int iNewVehicle = PossessionChangers[i]->ShouldChangePossession(Vehicles[(int)CurrentVehicle]);
			if (iNewVehicle != -1)
			{
				ChangePossession((POSSESSABLE_VEHICLES)iNewVehicle);
				return; // We're done, return out.
			}
		}
	}
}

void APossessionChangerManager::ForceChangePossession(POSSESSABLE_VEHICLES NewVehicle)
{
	ChangePossession(NewVehicle);
}

void APossessionChangerManager::ChangePossession(POSSESSABLE_VEHICLES NewVehicle)
{
	GetWorld()->GetFirstPlayerController()->Possess(Vehicles[(int)NewVehicle]);
	CurrentVehicle = NewVehicle;
}