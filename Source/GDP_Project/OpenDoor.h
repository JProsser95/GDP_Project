// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerVolume.h"
#include "Engine/World.h"
#include "OpenDoor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOpenRequest); // Create new delegate class to broadcast to blueprints.

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloseRequest); // Create new delegate class to broadcast to blueprints.


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GDP_PROJECT_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FOnOpenRequest OnOpenRequest; // Create instance of blueprint assignable class.
	
	UPROPERTY(BlueprintAssignable)
	FOnCloseRequest OnCloseRequest; // Create instance of blueprint assignable class.
private:
	UPROPERTY(EditAnywhere)
	float OpenAngle;
	
	UPROPERTY(EditAnywhere)
	ATriggerVolume* PressurePlate = nullptr;

	UPROPERTY(EditAnywhere)
	float DoorCloseDelay;

	float LastDoorOpenTime;

	void OpenDoor();
	void CloseDoor();

	float GetTotalMassOnPlate();
};
