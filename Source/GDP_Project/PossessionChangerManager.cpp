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

void APossessionChangerManager::ChangePossession()
{
	for (int i = 0; i < PossessionChangers.Num(); ++i)
	{
		int iNewVehicle = PossessionChangers[i]->ShouldChangePossession(Vehicles[(int)CurrentVehicle]);
		if (iNewVehicle != -1)
		{
			GetWorld()->GetFirstPlayerController()->Possess(Vehicles[iNewVehicle]);
			CurrentVehicle = (POSSESSABLE_VEHICLES)iNewVehicle;
		}
	}
}

