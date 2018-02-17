// Fill out your copyright notice in the Description page of Project Settings.

#include "TrackSwitcher.h"


// Sets default values
ATrackSwitcher::ATrackSwitcher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	TArray<UPrimitiveComponent*> components;

	GetOverlappingComponents(components);

	for (UPrimitiveComponent* pComponent : components)
	{
		if (pComponent->GetCollisionObjectType() == ECollisionChannel::ECC_Vehicle)
		{
			ToyTrain->TrackSwitcherHit(TrackSwitchNumber);
			Destroy();
		}
	}

}

