// Fill out your copyright notice in the Description page of Project Settings.

#include "GDP_ProjectGameModeBase.h"
#include "ToyPlane.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "TimerWidgetParent.h"
#include "Macros.h"

const int MAX_TIME(60);

void AGDP_ProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < Part::MAX; ++i) // Set all plane parts to not collected at the start of the game.
	{ 
		CollectedPlaneParts[i] = false;
	}

	iTimeLeft = 0.0f;
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

void AGDP_ProjectGameModeBase::SetVehicleHUD()
{
	if (VehicleWidget)
		return;

	VehicleWidget = CreateWidget<UUserWidget>(GetWorld(), ChangeVehicleHUDClass);
	if (VehicleWidget != nullptr)
	{
		VehicleWidget->AddToViewport();
	}
}

void AGDP_ProjectGameModeBase::RemoveVehicleHUD()
{
	if (VehicleWidget)
	{
		VehicleWidget->RemoveFromViewport();
		VehicleWidget = nullptr;
	}
}

void AGDP_ProjectGameModeBase::SetInteractionHUD()
{
	if (InteractionWidget)
		return;

	InteractionWidget = CreateWidget<UUserWidget>(GetWorld(), InteractionHUDClass);
	if (InteractionWidget != nullptr)
	{
		InteractionWidget->AddToViewport();
	}
}

void AGDP_ProjectGameModeBase::RemoveInteractionHUD()
{
	if (InteractionWidget)
	{
		InteractionWidget->RemoveFromViewport();
		InteractionWidget = nullptr;
	}
}

FString AGDP_ProjectGameModeBase::GetHintText()
{
	return HintText;
}

void AGDP_ProjectGameModeBase::SetHintHUD(const FString& strHintText)
{
	if (HintWidget)
		return;

	HintText = strHintText;

	HintWidget = CreateWidget<UUserWidget>(GetWorld(), HintHUDClass);
	if (HintWidget != nullptr)
	{
		HintWidget->AddToViewport();
	}
}

void AGDP_ProjectGameModeBase::RemoveHintHUD()
{
	HintText = "";

	if (HintWidget)
	{
		HintWidget->RemoveFromViewport();
		HintWidget = nullptr;
	}
}

void AGDP_ProjectGameModeBase::SetPlanePartCollected(Part PartCollected)
{
	CollectedPlaneParts[PartCollected] = true;
	DisplayPlanePartsCollected(false);
}

bool AGDP_ProjectGameModeBase::GetPlanePartCollected(Part PartCollected)
{
	return CollectedPlaneParts[PartCollected];
}

void AGDP_ProjectGameModeBase::DisplayPlanePartsCollected(bool bToggle)
{
	if (bToggle)
	{
		if (PlanePartsWidget != nullptr) // UI already exists, remove it.
		{
			PlanePartsWidget->RemoveFromViewport();
			PlanePartsWidget = nullptr;
		}
		else
		{
			DisplayPlanePartsWidget();
		}
	}
	else
	{
		DisplayPlanePartsWidget();
	}
}

void AGDP_ProjectGameModeBase::DisplayPlanePartsWidget()
{
	if (PlanePartsWidget != nullptr)
	{
		PlanePartsWidget->RemoveFromViewport();
		PlanePartsWidget = nullptr;
	}
	if (PlanePartsHUDClass != nullptr)
	{
		PlanePartsWidget = CreateWidget<UUserWidget>(GetWorld(), PlanePartsHUDClass);
		if (PlanePartsWidget != nullptr)
		{
			PlanePartsWidget->AddToViewport();
		}
	}
}

void AGDP_ProjectGameModeBase::DisplayTimer()
{
	if (TimerWidget != nullptr)
	{
		TimerWidget->RemoveFromViewport();
		TimerWidget = nullptr;
	}
	if (TimePuzzleHUDClass != nullptr)
	{
		TimerWidget = CreateWidget<UUserWidget>(GetWorld(), TimePuzzleHUDClass);
		if (TimerWidget != nullptr)
		{
			TimerWidget->AddToViewport();
		}
	}
}

void AGDP_ProjectGameModeBase::RemoveTimerWidget()
{
	if (TimerWidget != nullptr)
	{
		TimerWidget->RemoveFromViewport();
		TimerWidget = nullptr;
	}
}


void AGDP_ProjectGameModeBase::BeginTimer()
{
	OUTPUT_STRING("Begin Timer");
	iTimeLeft = MAX_TIME;
	GetWorldTimerManager().SetTimer(Timer, this, &AGDP_ProjectGameModeBase::UpdateTimer, 1.0f, true, 0.0f);
}

void AGDP_ProjectGameModeBase::UpdateTimer()
{
	OUTPUT_INT(iTimeLeft);
	if (--iTimeLeft <= 0) 
	{
		GetWorldTimerManager().ClearTimer(Timer);
	}
	DisplayTimer();
}




