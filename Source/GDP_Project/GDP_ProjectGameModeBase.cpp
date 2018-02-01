// Fill out your copyright notice in the Description page of Project Settings.

#include "GDP_ProjectGameModeBase.h"
#include "ToyPlane.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Macros.h"


void AGDP_ProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < NUMBEROFPLANEPARTS; ++i) // Set all plane parts to not collected at the start of the game.
	{ 
		CollectedPlaneParts[i] = false;
	}
}

void AGDP_ProjectGameModeBase::ChangeHUD(const FString& name)
{
	if (name == "ToyPlane") 
	{
		ChangeMenuWidget(PlaneHUDClass);
	} 
	else if (name == "ToyTrain")
	{
		ChangeMenuWidget(TrainHUDClass);
	} 
	else if (name == "ToyCar")
	{
		ChangeMenuWidget(CarHUDClass);
	}
	else if (name == "HUD")
	{
		ChangeMenuWidget(PlanePartsHUDClass);
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

void AGDP_ProjectGameModeBase::AddHUD()
{
	VehicleWidget = CreateWidget<UUserWidget>(GetWorld(), ChangeVehicleHUDClass);
	if (VehicleWidget != nullptr)
	{
		VehicleWidget->AddToViewport();
	}
}

void AGDP_ProjectGameModeBase::RemoveHUD()
{
	VehicleWidget->RemoveFromViewport();
	VehicleWidget = nullptr;
}

void AGDP_ProjectGameModeBase::SetPlanePartCollected(PlaneParts PartCollected)
{
	CollectedPlaneParts[PartCollected] = true;
	//UE_LOG(LogTemp, Warning, TEXT("Plane Parts Collected: %d, %d, %d, %d, %d"), CollectedPlaneParts[0], CollectedPlaneParts[1], CollectedPlaneParts[2], CollectedPlaneParts[3], CollectedPlaneParts[4]);

	ChangeHUD("HUD");
}