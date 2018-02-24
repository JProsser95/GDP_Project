// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PossessionChanger.generated.h"

UENUM()
enum class POSSESSABLE_VEHICLES : uint8
{
	Car		UMETA(DisplayName = "Car"),
	Plane	UMETA(DisplayName = "Plane"),
	Train	UMETA(DisplayName = "Train")
};

UCLASS()
class GDP_PROJECT_API APossessionChanger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APossessionChanger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	POSSESSABLE_VEHICLES VehicleToChangeFrom;

	UPROPERTY(EditAnywhere)
	POSSESSABLE_VEHICLES VehicleToChangeTo;

	UPROPERTY(EditAnywhere)
	bool IsAPuzzleSolution;
	
	int ShouldChangePossession(APawn* FromVehicle);
	bool PuzzleSolutionOverlapped(APawn* FromVehicle);
};
