// Fill out your copyright notice in the Description page of Project Settings.

#include "PushableObject.h"


// Sets default values for this component's properties
UPushableObject::UPushableObject()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPushableObject::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UPushableObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPushableObject::Push(const FVector& vObjectPosition)
{
	FVector vDirection = GetOwner()->GetActorLocation() - vObjectPosition; // Get the direction we want to push in.
	vDirection.Normalize(); // Convert to a unit vector to limit movement.
	vDirection.Z = 0.0f; // Nullify the Z component. We don't want to be able to push the object into the floor.
	
	/// Add on the position and update the Actor.
	FVector vPosition = GetOwner()->GetActorLocation() + vDirection;
	GetOwner()->SetActorLocation(vPosition);
}