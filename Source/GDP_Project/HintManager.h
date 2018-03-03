// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerBox.h"
#include "HintManager.generated.h"

UCLASS()
class GDP_PROJECT_API AHintManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHintManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetUI(int iMessage);
	void RemoveUI();

	int m_iActiveTrigger;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TArray<ATriggerBox*> Triggers;
	

	UPROPERTY(EditAnywhere)
	TArray<FString> Messages;
};
