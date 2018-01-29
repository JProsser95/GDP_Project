// Fill out your copyright notice in the Description page of Project Settings.

#include "GDP_ProjectGameModeBase.h"
#include "ToyPlane.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"


void AGDP_ProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void AGDP_ProjectGameModeBase::ChangeHUD(FString name)
{

	if (name == "ToyPlane") 
	{
		ChangeMenuWidget(PlaneHUDClass);
	}
	
	if (name == "ToyTrain")
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