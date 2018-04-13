// Fill out your copyright notice in the Description page of Project Settings.

#include "FallingRespawner.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "ToyCar.h"

// Sets default values
AFallingRespawner::AFallingRespawner()
	: bIsActive(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	//Trigger Box for activating this spawn point
	GroundTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	GroundTriggerBox->SetupAttachment(RootComponent);
	GroundTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	RespawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndPointComponent"));
	RespawnPoint->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AFallingRespawner::BeginPlay()
{
	Super::BeginPlay();
	
	// Get the ToyCar that is now in the scene
	for (TActorIterator<AToyCar> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		pCar = (*ActorItr);
	}
}

// Called every frame
void AFallingRespawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TriggerVolume->IsOverlappingActor(pCar))
	{
		bIsActive = true;
	}

	if (bIsActive)
	{
		if (GroundTriggerBox->IsOverlappingActor(pCar))
		{
			pCar->SetActorRelativeLocation(RespawnPoint->GetComponentLocation(), false, NULL, ETeleportType::TeleportPhysics);
			pCar->SetActorRotation(FQuat(RespawnPoint->GetComponentRotation()), ETeleportType::TeleportPhysics);
		}
	}
}

