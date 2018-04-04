// Fill out your copyright notice in the Description page of Project Settings.

#include "PlanePointManager.h"
#include "ToyPlane.h"
#include "Macros.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "AchievementManager.h"


// Sets default values
APlanePointManager::APlanePointManager()
	:ToyPlane(nullptr), BoostIncrement(20.0f), m_ePlaneLocation(CENTRE), m_fEndingDelay(1.0f)
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
	if (!Actors.Num() || !Actors[0])
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Plane Points Not Setup Correctly!"));
	}
}

// Called every frame
void APlanePointManager::Tick(float DeltaTime)
{
	if (Actors.Num())
	{
		for (int i(0); i < Actors.Num(); ++i)
		{
			if (Actors[i]->IsOverlappingActor(ToyPlane))
			{
				ToyPlane->UpdateCurrentBoost(BoostIncrement);
				GetWorld()->DestroyActor(Actors[i]);
				Actors.RemoveAt(i);
			}
		}
	}
	else
	{
		if (m_fEndingDelay <= 0.0f)
			AllPointsCollected(DeltaTime);
		else
			m_fEndingDelay -= DeltaTime;
	}
}

void APlanePointManager::AllPointsCollected(float DeltaTime)
{
	for (TActorIterator<AAchievementManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->EarnAchievement(AchievementName::POPPIN);
	}

	if (m_ePlaneLocation == CENTRE)
	{
		FVector Location(12578.0f, 2053.0f, 1150.0f);
		if ((ToyPlane->GetActorLocation() - Location).Size() > 15.0f)
			ToyPlane->FlyTowards(Location, DeltaTime);
		else
			m_ePlaneLocation = OUT_WINDOW;
		
	}

	else
	{
		ToyPlane->StartBoost();
		ToyPlane->FlyTowards(FVector(12300.0f, -11000.0f, 1150.0f), DeltaTime);
	}
}