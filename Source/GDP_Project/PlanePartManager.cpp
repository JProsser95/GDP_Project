// Fill out your copyright notice in the Description page of Project Settings.

#include "PlanePartManager.h"
#include "PlanePart.h"
#include "Macros.h"

// Sets default values
APlanePartManager::APlanePartManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlanePartManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlanePartManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlaneParts.Num() == 0)
		OUTPUT_STRING("All Parts Collected");
}

