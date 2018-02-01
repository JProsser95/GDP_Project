// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerVolume.h"
#include "Engine/World.h"
#include "Engine/SpotLight.h"
#include "TriggerManager.generated.h"

UCLASS()
class GDP_PROJECT_API ATriggerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriggerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TArray<ATriggerVolume*> Triggers;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> TriggersCausers;

	UPROPERTY(EditAnywhere)
	ASpotLight* Light;

private:
	int m_iActiveTrigger;
	
	void ChangeTrigger();
};
