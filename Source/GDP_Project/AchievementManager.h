// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AchievementManager.generated.h"

UENUM(BlueprintType)
enum AchievementName
{
	PLANE_WING,
	PLANE_HULL,
	PLANE_PROP,
	VASE_BREAK,
	GOAL,
	SMASH_ALL_VASES,
	OFF_THE_RAILS,
	FAST_RING_TIME,
	WINGING,
	POPPIN,
	ESCAPIN,
	PLAYROOM_COMPLETE,
	LOUNGE_COMPLETE,
	PLANE_CRASH,
	RAID_NIGHT,
	MAX_SIZE
};

UCLASS()
class GDP_PROJECT_API AAchievementManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAchievementManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Achievement")
	TArray<bool> bIsAchievementAlreadyEarned;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Achievement")
	void EarnAchievement(AchievementName name);

	UFUNCTION(BlueprintImplementableEvent, Category = "Achievement")
	void RenderAchievement(AchievementName name);
};
