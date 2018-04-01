// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AchievementSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class GDP_PROJECT_API UAchievementSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UAchievementSaveGame();

	UPROPERTY(VisibleAnywhere, Category = Basic)
	TArray<bool> bIsAchievementAlreadyEarned;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
	
	
};
