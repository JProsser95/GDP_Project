// Fill out your copyright notice in the Description page of Project Settings.

#include "HintManager.h"
#include "GDP_ProjectGameModeBase.h"
#include "PossessableActorComponent.h"

// Sets default values
AHintManager::AHintManager()
	: m_iActiveTrigger(-1)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHintManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (Triggers.Num() != Messages.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Hint Manager does not have the same number of triggers and messages. This will cause errors."));
	}
}

// Called every frame
void AHintManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_iActiveTrigger == -1)
	{
		for (int i = 0; i < Triggers.Num(); ++i)
		{
			TArray<AActor*> OverlappedActors;
			Triggers[i]->GetOverlappingActors(OverlappedActors);
			for (AActor* Actor : OverlappedActors)
			{
				if (Actor->FindComponentByClass<UPossessableActorComponent>())
				{
					SetUI(i);
					m_iActiveTrigger = i;
					return; // No point searching anymore.
				}
			}
		}
		m_iActiveTrigger = -1;
	}
	else
	{
		bool bVehicleExitedCollision = true;
		TArray<AActor*> OverlappedActors;
		Triggers[m_iActiveTrigger]->GetOverlappingActors(OverlappedActors);
		for (AActor* Actor : OverlappedActors)
		{
			if (Actor->FindComponentByClass<UPossessableActorComponent>())
			{
				bVehicleExitedCollision = false;
			}
		}
		if (bVehicleExitedCollision)
		{
			Triggers[m_iActiveTrigger]->SetActorEnableCollision(false);
			m_iActiveTrigger = -1;
			RemoveUI();
		}
	}
}

void AHintManager::SetUI(int iMessage)
{
	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->SetHintHUD(Messages[iMessage]);
}

void AHintManager::RemoveUI()
{
	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->RemoveHintHUD();
}