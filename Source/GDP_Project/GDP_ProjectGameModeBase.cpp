// Fill out your copyright notice in the Description page of Project Settings.

#include "GDP_ProjectGameModeBase.h"
#include "ToyPlane.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Macros.h"


void AGDP_ProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	OUTPUT_STRING("IN BEGIN");
}

void AGDP_ProjectGameModeBase::ChangeHUD(FString test)
{
	OUTPUT_STRING("CHANGE HUD");

	if (test == "Plane") 
	{
		ChangeMenuWidget(PlaneHUDClass);
	}
	else
	{
		ChangeMenuWidget(TrainHUDClass);
	}

}

void AGDP_ProjectGameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}
