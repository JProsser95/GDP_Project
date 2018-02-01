// Fill out your copyright notice in the Description page of Project Settings.

#include "CollectableObject.h"


// Sets default values for this component's properties
UCollectableObject::UCollectableObject()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	if (PlanePartName == "PlanePart1")
		PlanePart = PlanePart1;
}


// Called when the game starts
void UCollectableObject::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UCollectableObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	AActor* pPlayer = GetWorld()->GetFirstPlayerController()->GetPawn();

	TArray<AActor*> Actors;

	GetOwner()->GetOverlappingActors(Actors);

	if (GetOwner()->IsOverlappingActor(pPlayer) || Actors.Num())
	{
		AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
		GameMode->SetPlanePartCollected(PlanePart);
		GetWorld()->DestroyActor(GetOwner());
	}
}

