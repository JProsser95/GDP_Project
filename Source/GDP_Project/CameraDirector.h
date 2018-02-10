// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraDirector.generated.h"

UCLASS()
class GDP_PROJECT_API ACameraDirector : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ACameraDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	AActor* CarCamera;

	UPROPERTY(EditAnywhere)
	AActor* GarageCamera;

	void BeginCameraChange();

	float TimeToNextCameraChange;

private:

	bool bCameraChange;

};
