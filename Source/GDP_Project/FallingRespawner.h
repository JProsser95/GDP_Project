// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerVolume.h"
#include "FallingRespawner.generated.h"

class UBoxComponent;
class USceneComponent;
class AToyCar;

UCLASS()
class GDP_PROJECT_API AFallingRespawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFallingRespawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	USceneComponent* RespawnPoint;

	UPROPERTY(EditAnywhere)
	UBoxComponent*  GroundTriggerBox;

	AToyCar* pCar;

	UPROPERTY(EditAnywhere)
	ATriggerVolume* TriggerVolume;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool bIsActive;
	
};
