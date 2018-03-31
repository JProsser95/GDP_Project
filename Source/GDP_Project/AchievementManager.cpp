// Fill out your copyright notice in the Description page of Project Settings.

#include "AchievementManager.h"

// Sets default values
AAchievementManager::AAchievementManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	for (int i(0); i < AchievementName::MAX_SIZE; ++i)
		bIsAchievementAlreadyEarned.Add(false);

}

// Called when the game starts or when spawned
void AAchievementManager::BeginPlay()
{
	Super::BeginPlay();
	
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
}

