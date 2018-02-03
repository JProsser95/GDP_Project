// Fill out your copyright notice in the Description page of Project Settings.

#include "CollectableObject.h"


// Sets default values for this component's properties
UCollectableObject::UCollectableObject()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCollectableObject::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (PlanePartName == "PlanePart1")
		PlanePart = PlanePart1;
	else if (PlanePartName == "PlanePart2")
		PlanePart = PlanePart2;
	else if (PlanePartName == "PlanePart3")
		PlanePart = PlanePart3;
	else if (PlanePartName == "PlanePart4")
		PlanePart = PlanePart4;
	else if (PlanePartName == "PlanePart5")
		PlanePart = PlanePart5;
}


// Called every frame
void UCollectableObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<UPrimitiveComponent*> components;

	GetOwner()->GetOverlappingComponents(components);

	for (UPrimitiveComponent* pComponent : components)
	{
		if (pComponent->GetCollisionObjectType() == ECollisionChannel::ECC_Vehicle)
		{
			AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
			GameMode->SetPlanePartCollected(PlanePart);
			GetWorld()->DestroyActor(GetOwner());
		}
	}
}

