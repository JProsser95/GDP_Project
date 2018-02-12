// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "ToyPlane.h"
#include "PossessableActorComponent.h"
#include "PlanePointManager.generated.h"


//class AToyPlane;

UCLASS()
class GDP_PROJECT_API APlanePointManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlanePointManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> Actors;

	UPROPERTY(EditAnywhere)
	AToyPlane* ToyPlane;

	void AllPointsCollected();
private:
	int GetVisibleActors();
};
