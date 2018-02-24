// Fill out your copyright notice in the Description page of Project Settings.

#include "PossessionChanger.h"
#include "UObject/ConstructorHelpers.h"
#include "ToyCar.h"
#include "ToyPlane.h"
#include "ToyTrain.h"


// Sets default values
APossessionChanger::APossessionChanger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
