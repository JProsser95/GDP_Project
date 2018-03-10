// Fill out your copyright notice in the Description page of Project Settings.

#include "PossessionChanger.h"
#include "UObject/ConstructorHelpers.h"
#include "ToyCar.h"
#include "ToyPlane.h"
#include "GDP_ProjectGameModeBase.h"
#include "Components/BoxComponent.h"

// Sets default values
APossessionChanger::APossessionChanger()
	:VehicleToChangeTo(POSSESSABLE_VEHICLES::Plane)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Mesh
	HotSwapMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HotSwapMeshComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetHotSwap(TEXT("StaticMesh'/Game/Assets/Hotswap/Hotswap.Hotswap'"));
	if (MeshAssetHotSwap.Object)
		HotSwapMeshComponent->SetStaticMesh(MeshAssetHotSwap.Object);

	RootComponent = HotSwapMeshComponent;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	TriggerBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	TriggerBox->SetWorldScale3D(FVector(0.4f, 0.375f, 0.325f));
	TriggerBox->SetRelativeLocation(FVector (-40.0f, 0.0f, 40.0f));
}

// Called when the game starts or when spawned
void APossessionChanger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APossessionChanger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int APossessionChanger::ShouldChangePossession(APawn* FromVehicle)
{
	if (IsOverlappingActor(FromVehicle))
		return (int)VehicleToChangeTo;

	return -1;
}

bool APossessionChanger::PuzzleSolutionOverlapped(APawn * FromVehicle)
{
	return IsAPuzzleSolution && IsOverlappingActor(FromVehicle);
}
