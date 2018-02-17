// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "CeilingFan.generated.h"

UCLASS()
class GDP_PROJECT_API ACeilingFan : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACeilingFan();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CeilingCylinderMeshComponent;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CeilingFanMeshComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
