// Fill out your copyright notice in the Description page of Project Settings.

#include "GDP_ProjectGameModeBase.h"
#include "ToyPlane.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"



void AGDP_ProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	AToyPlane* MyPlane = Cast<AToyPlane>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerHUDClass != nullptr) 
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);

		if (CurrentWidget != nullptr) 
		{
			CurrentWidget->AddToViewport();
		}
	}
}
