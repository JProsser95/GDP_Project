// Fill out your copyright notice in the Description page of Project Settings.

#include "TriggerManager.h"


// Sets default values
ATriggerManager::ATriggerManager()
	: m_iActiveTrigger(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATriggerManager::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 1; i < Triggers.Num(); ++i)
	{
		Triggers[i]->SetActorEnableCollision(false);
	}
	
}

// Called every frame
void ATriggerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_iActiveTrigger >= Triggers.Num()) // No more triggers, stop updating.
		return;

	AActor* pActor = TriggersCausers[m_iActiveTrigger] == nullptr ? GetWorld()->GetFirstPlayerController()->GetPawn() : TriggersCausers[m_iActiveTrigger];

	if (Triggers[m_iActiveTrigger]->IsOverlappingActor(pActor))
	{
		if (++m_iActiveTrigger < Triggers.Num())
			ChangeTrigger();
		else
			Light->SetActorHiddenInGame(true);
	}
}

void ATriggerManager::ChangeTrigger()
{
	FVector newLightPosition(Triggers[m_iActiveTrigger]->GetActorLocation());
	newLightPosition.Z += 125.0f;
	Light->SetActorLocation(newLightPosition);

	Triggers[m_iActiveTrigger-1]->SetActorEnableCollision(false);
	Triggers[m_iActiveTrigger]->SetActorEnableCollision(true);
}