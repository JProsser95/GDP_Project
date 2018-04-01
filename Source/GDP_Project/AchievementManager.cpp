// Fill out your copyright notice in the Description page of Project Settings.

#include "AchievementManager.h"
#include "Kismet/GameplayStatics.h"
#include "AchievementSaveGame.h"
#include "Macros.h"

// Sets default values
AAchievementManager::AAchievementManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAchievementManager::BeginPlay()
{
	Super::BeginPlay();

	UAchievementSaveGame* LoadGameInstance = Cast<UAchievementSaveGame>(UGameplayStatics::CreateSaveGameObject(UAchievementSaveGame::StaticClass()));
	LoadGameInstance = Cast<UAchievementSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));
	bIsAchievementAlreadyEarned = LoadGameInstance->bIsAchievementAlreadyEarned;

	if (LoadGameInstance->bIsAchievementAlreadyEarned.Num() != 0)
	{
		for (int i(0); i < AchievementName::MAX_SIZE; ++i)
			bIsAchievementAlreadyEarned.Add(LoadGameInstance->bIsAchievementAlreadyEarned[i]);

		OUTPUT_STRING("LOADED");
	}
	else
	{
		for (int i(0); i < AchievementName::MAX_SIZE; ++i)
			bIsAchievementAlreadyEarned.Add(false);

		OUTPUT_STRING("NOT LOADED");

	}
}

// Called every frame
void AAchievementManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAchievementManager::EarnAchievement(AchievementName name)
{
	if (bIsAchievementAlreadyEarned[name])
		return;

	bIsAchievementAlreadyEarned[name] = true;
	RenderAchievement(name);
	UAchievementSaveGame* SaveGameInstance = Cast<UAchievementSaveGame>(UGameplayStatics::CreateSaveGameObject(UAchievementSaveGame::StaticClass()));

	for (int i(0); i < AchievementName::MAX_SIZE; ++i)
		SaveGameInstance->bIsAchievementAlreadyEarned[i] = bIsAchievementAlreadyEarned[i];

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
}

