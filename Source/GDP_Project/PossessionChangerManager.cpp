// Fill out your copyright notice in the Description page of Project Settings.

#include "PossessionChangerManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "ToyCar.h"
#include "GDP_ProjectGameModeBase.h"

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
	
	// Get the ToyCar that is now in the scene
	for (TActorIterator<AToyCar> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		(*ActorItr)->SetPossessionChangeManager(this);
		Vehicles[(int)POSSESSABLE_VEHICLES::Car] = *ActorItr;
	}
}

// Called every frame
void APossessionChangerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Vehicles[(int)POSSESSABLE_VEHICLES::Car])
		CheckPossessionPads(false); // Used to check and update the UI
}

void APossessionChangerManager::CheckPossessionPads(bool bChangePossession)
{
	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	for (int i = 0; i < PossessionChangers.Num(); ++i)
	{
		if (PossessionChangers[i]->VehicleToChangeFrom == CurrentVehicle)
		{
			int iNewVehicle = PossessionChangers[i]->ShouldChangePossession(Vehicles[(int)CurrentVehicle]);
			if (iNewVehicle != -1)
			{
				if (bChangePossession)
				{
					ChangePossession((POSSESSABLE_VEHICLES)iNewVehicle);
				}
				else if(GameMode)
					GameMode->SetVehicleHUD();

				return; // We're done, return out.
			}
		}
	}

	if (GameMode)
		GameMode->RemoveVehicleHUD();
}

void APossessionChangerManager::ForceChangePossession(POSSESSABLE_VEHICLES NewVehicle)
{
	ChangePossession(NewVehicle);
}

bool APossessionChangerManager::PuzzleSolutionPadIsOverlapped()
{
	for (int i = 0; i < PossessionChangers.Num(); ++i)
	{
		if (PossessionChangers[i]->VehicleToChangeFrom == CurrentVehicle)
		{
			if(PossessionChangers[i]->PuzzleSolutionOverlapped(Vehicles[(int)CurrentVehicle]))
			{
				return true;
			}
		}
	}
	return false;
}

void APossessionChangerManager::ChangePossession(POSSESSABLE_VEHICLES NewVehicle)
{
	GetWorld()->GetFirstPlayerController()->Possess(Vehicles[(int)NewVehicle]);
	CurrentVehicle = NewVehicle;
}