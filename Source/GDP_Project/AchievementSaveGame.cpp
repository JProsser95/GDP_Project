// Fill out your copyright notice in the Description page of Project Settings.

#include "AchievementSaveGame.h"
#include "AchievementManager.h"
#include "GDP_ProjectGameModeBase.h"



UAchievementSaveGame::UAchievementSaveGame()
{
	SaveSlotName = TEXT("MainSaveSlot");
	UserIndex = 0;
	for (int i(0); i < AchievementName::MAX_SIZE; ++i)
		bIsAchievementAlreadyEarned.Add(false);
}

