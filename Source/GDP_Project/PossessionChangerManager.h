// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "PossessionChanger.h"
#include "PossessionChangerManager.generated.h"


UCLASS()
class GDP_PROJECT_API APossessionChangerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APossessionChangerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	POSSESSABLE_VEHICLES CurrentVehicle;

	void ChangePossession(POSSESSABLE_VEHICLES NewVehicle);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CheckPossessionPads(bool bChangePossession = true);

	void ForceChangePossession(POSSESSABLE_VEHICLES NewVehicle);

	bool PuzzleSolutionPadIsOverlapped();

	void FindVehiclesAndPossessionChangers();

	UPROPERTY(EditAnywhere)
	TArray<APawn*> Vehicles;
	
	UPROPERTY(EditAnywhere)
	TArray<APossessionChanger*> PossessionChangers;
};
