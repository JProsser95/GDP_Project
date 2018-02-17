// Fill out your copyright notice in the Description page of Project Settings.

#include "TrackSwappingManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATrackSwappingManager::ATrackSwappingManager()
	: MaterialOff(nullptr), MaterialOn(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RefMaterialOff(TEXT("Material'/Game/EnviBlockout/OldBlockout/01_-_Default.01_-_Default'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RefMaterialOn(TEXT("Material'/Game/EnviBlockout/OldBlockout/02_-_Default1.02_-_Default1'"));


	if (RefMaterialOff.Object)
		MaterialOff = RefMaterialOff.Object;

	if (RefMaterialOn.Object)
		MaterialOn = RefMaterialOn.Object;

}

// Called when the game starts or when spawned
void ATrackSwappingManager::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0; i < Swappers.Num(); ++i)
	{
		Swappers[i]->FindComponentByClass<UStaticMeshComponent>()->SetMaterial(0, MaterialOff);
	}
}

// Called every frame
void ATrackSwappingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int ATrackSwappingManager::GetNearestSwapper(AActor* ToyTrain)
{
	for (int i = 0; i < Swappers.Num(); ++i)
	{
		if (Swappers[i]->IsOverlappingActor(ToyTrain))
		{
			if(Swappers[i]->FindComponentByClass<UStaticMeshComponent>()->GetMaterial(0) == MaterialOff)
				Swappers[i]->FindComponentByClass<UStaticMeshComponent>()->SetMaterial(0, MaterialOn);
			else
				Swappers[i]->FindComponentByClass<UStaticMeshComponent>()->SetMaterial(0, MaterialOff);
			return i;
		}
	}

	return -1;
}
