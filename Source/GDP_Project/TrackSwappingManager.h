// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrackSwappingManager.generated.h"

UCLASS()
class GDP_PROJECT_API ATrackSwappingManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrackSwappingManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int GetNearestSwapper(AActor* ToyTrain);


	UPROPERTY(EditAnywhere)
	TArray<AActor*> Swappers;

	UMaterialInterface* MaterialOff;
	UMaterialInterface* MaterialOn;
	
};
