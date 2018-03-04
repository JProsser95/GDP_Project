// Fill out your copyright notice in the Description page of Project Settings.

#include "HintsWidgetParent.h"
#include "GDP_ProjectGameModeBase.h"

/** Retrieves FText value currently held in DisplayText */
FText UHintsWidgetParent::GetHintText()
{
	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();

	HintText = FText::FromString(GameMode->GetHintText());

	return HintText;
}

/** Assigns passed FText to DisplayText */
void UHintsWidgetParent::SetDisplayText(const FText& NewDisplayText)
{
	HintText = NewDisplayText;
}