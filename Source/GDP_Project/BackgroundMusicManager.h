// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BackgroundMusicManager.generated.h"

class USoundCue;
class UAudioComponent;

UENUM()
enum Sounds
{
	MAIN,
	CAMERA_PUZZLE,
	TIMER_PUZZLE,
	TRAIN_PUZZLE
};

UCLASS()
class GDP_PROJECT_API ABackgroundMusicManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABackgroundMusicManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	Sounds currentSoundCue;

	//UPROPERTY(EditAnywhere, Category = "Sounds")
	TArray<USoundCue*> AudioCues;

	UPROPERTY(Category = Sound, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* AudioComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Background Sound")
	void SetSound(Sounds sound, float FadeInTime = 1.0f);

};