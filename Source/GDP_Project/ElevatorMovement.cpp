// Fill out your copyright notice in the Description page of Project Settings.

#include "ElevatorMovement.h"


// Sets default values for this component's properties
UElevatorMovement::UElevatorMovement()
	: TriggerVolume(nullptr), CurrentLiftTime(0.0f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UElevatorMovement::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	if (!TriggerVolume)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find TriggerVolume in ElevatorMovement"));
	}

	StartPosition = GetOwner()->GetActorLocation();
}


// Called every frame
void UElevatorMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (TriggerVolume->IsOverlappingActor(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		ActivateLift(DeltaTime);
	}
	else
	{
		DeactivateLift(DeltaTime);
	}
}

void UElevatorMovement::ActivateLift(float fDeltaTime)
{
	CurrentLiftTime += fDeltaTime;

	if (CurrentLiftTime > MovementTime)
	{
		CurrentLiftTime = MovementTime;
	}
	UpdateLift();
}

void UElevatorMovement::DeactivateLift(float fDeltaTime)
{
	CurrentLiftTime -= fDeltaTime;

	if (CurrentLiftTime < 0)
	{
		CurrentLiftTime = 0;
	}
	UpdateLift();
}

void UElevatorMovement::UpdateLift()
{
	FVector NewPosition = StartPosition;
	NewPosition.Z += ((NewHeight - StartPosition.Z) * CurrentLiftTime / MovementTime);

	GetOwner()->SetActorLocation(NewPosition);
}