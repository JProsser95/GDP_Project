// Fill out your copyright notice in the Description page of Project Settings.

#include "TimerWidgetParent.h"
#include "GDP_ProjectGameModeBase.h"



FText UTimerWidgetParent::GetDisplayText()
{
	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	FString strHUDOutput = "";

	strHUDOutput = "Time: " + FString::FromInt(GameMode->GetTimeLeft());

	DisplayText = FText::FromString(strHUDOutput);

	return DisplayText;
}
