// Fill out your copyright notice in the Description page of Project Settings.

#include "PlanePointManager.h"
#include "ToyPlane.h"
#include "Macros.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "AchievementManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"


// Sets default values
APlanePointManager::APlanePointManager()
	:ToyPlane(nullptr), BoostIncrement(20.0f), m_fEndingDelay(1.0f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USoundCue> powerUpSound(TEXT("/Game/Sounds/Power_Up_Cue"));

	PowerUpSound = powerUpSound.Object;

	// Create an audio component, the audio component wraps the Cue
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

	// I don't want the sound playing the moment it's created.
	AudioComponent->bAutoActivate = false;

	AudioComponent->SetSound(PowerUpSound);

	AudioComponent->SetVolumeMultiplier(2.0f);
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

				AudioComponent->Play();
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
}