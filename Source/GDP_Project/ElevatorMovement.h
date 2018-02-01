// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Engine/TriggerVolume.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "ElevatorMovement.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GDP_PROJECT_API UElevatorMovement : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UElevatorMovement();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere)
	ATriggerVolume* TriggerVolume;
	
	FVector StartPosition;

	UPROPERTY(EditAnywhere)
	float NewHeight;

	UPROPERTY(EditAnywhere)
	float MovementTime;
	float CurrentLiftTime;

	void ActivateLift(float fDeltaTime);
	void DeactivateLift(float fDeltaTime);
	void UpdateLift();
};
