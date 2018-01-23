// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GDP_ProjectGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GDP_PROJECT_API AGDP_ProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
public:




protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Plane", Meta = (BlueprintProtected = true))
	TSubclassOf<class UUserWidget> PlayerHUDClass;

	UPROPERTY()
	class UUserWidget* CurrentWidget;
	

	
};
