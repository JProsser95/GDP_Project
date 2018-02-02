// Fill out your copyright notice in the Description page of Project Settings.

#include "HUDWidget.h"
#include "GDP_ProjectGameModeBase.h"

/** Retrieves FText value currently held in DisplayText */
FText UHUDWidget::GetDisplayText() 
{
	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	FString strHUDOutput = "";
	for (int i = 0; i < PlaneParts_MAX; ++i)
	{
		strHUDOutput += "PlanePart" + FString::FromInt(i) + ": " + FString::FromInt(GameMode->GetPlanePartCollected((PlaneParts)i)) + "\n";
	}

	DisplayText = FText::FromString(strHUDOutput);

	return DisplayText;
}

/** Assigns passed FText to DisplayText */
void UHUDWidget::SetDisplayText(const FText& NewDisplayText) 
{
	DisplayText = NewDisplayText;
}


