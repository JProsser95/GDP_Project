// Fill out your copyright notice in the Description page of Project Settings.

#include "TrackSwitcher.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values
ATrackSwitcher::ATrackSwitcher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USoundCue> collectSound(TEXT("/Game/Sounds/Button_Cue"));

	ButtonSound = collectSound.Object;

	// Create an audio component, the audio component wraps the Cue
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

	// I don't want the sound playing the moment it's created.
	AudioComponent->bAutoActivate = false;

	AudioComponent->SetSound(ButtonSound);

	AudioComponent->SetVolumeMultiplier(2.0f);
}

// Called when the game starts or when spawned
void ATrackSwitcher::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrackSwitcher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ToyTrain->OnFailureTrainLine()) // If the train is already on the failure line then there's no point updating anything
	{
		TArray<UPrimitiveComponent*> components;

		GetOverlappingComponents(components);

		for (UPrimitiveComponent* pComponent : components)
		{
			if (pComponent->GetCollisionObjectType() == ECollisionChannel::ECC_Vehicle)
			{
				ToyTrain->TrackSwitcherHit(TrackSwitchNumber);
				
				AudioComponent->Play();

				this->SetActorEnableCollision(false);
				this->SetActorHiddenInGame(true);
			}
		}
	}

}

