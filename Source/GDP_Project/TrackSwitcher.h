// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ToyTrain.h"
#include "TrackSwitcher.generated.h"

class USoundCue;
class UAudioComponent;

UCLASS()
class GDP_PROJECT_API ATrackSwitcher : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrackSwitcher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	AToyTrain* ToyTrain;

	UPROPERTY(EditAnywhere)
	int TrackSwitchNumber;
	
	UAudioComponent* AudioComponent;
	USoundCue* ButtonSound;
};
