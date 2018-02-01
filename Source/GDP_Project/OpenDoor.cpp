// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
	: OpenAngle(-90.0f), DoorCloseDelay(1.0f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	//ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	//PressurePlate->IsOverlappingActor(ActorThatOpens)
	if (GetTotalMassOnPlate() >= 50.0f)
	{
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}
	else
		CloseDoor();
}

void UOpenDoor::OpenDoor()
{
	// Get the owning actor
	//AActor* Owner = GetOwner();
	//
	//// Create a new rotation
	//FRotator Rotation = FRotator(0.0f, OpenAngle, 0.0f);
	//
	//// Assign the rotation to the owning actor.
	//Owner->SetActorRotation(Rotation);

	OnOpenRequest.Broadcast(); // Send message to blueprints.
}

void UOpenDoor::CloseDoor()
{
	//// Get the owning actor
	//AActor* Owner = GetOwner();
	//
	//// Create a new rotation
	//FRotator Rotation = FRotator(0.0f, 0.0f, 0.0f);
	//
	//// Assign the rotation to the owning actor.
	//Owner->SetActorRotation(Rotation);
	OnCloseRequest.Broadcast();
}

float UOpenDoor::GetTotalMassOnPlate()
{
	float fTotalMass = 0.0f;
	TArray<AActor*> Actors;

	PressurePlate->GetOverlappingActors(Actors);
	
	for (AActor* Actor : Actors)
	{
		fTotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%fKG on plate"), fTotalMass);
	}

	return fTotalMass;
}