// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReturnPortal.generated.h"

class UWidgetComponent;
class UBoxComponent;
class USceneComponent;


UCLASS()
class GDP_PROJECT_API AReturnPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReturnPortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	USceneComponent* WarpRoomReturnPoint;


	UPROPERTY(EditAnywhere)
	UWidgetComponent* LoungeReturnPointWidget;

	//UPROPERTY(EditAnywhere)
	//UWidgetComponent* TimerPuzzleCompleteWidget;
	//
	//UPROPERTY(EditAnywhere)
	//UWidgetComponent* FrictionPuzzleCompleteWidget;
	//
	//UPROPERTY(EditAnywhere)
	//UWidgetComponent* LavaPuzzleCompleteWidget;


	UPROPERTY(EditAnywhere)
	UBoxComponent* LoungeReturnPointTriggerBox;

	//UPROPERTY(EditAnywhere)
	//UBoxComponent* TimerPuzzleTriggerBox;
	//
	//UPROPERTY(EditAnywhere)
	//UBoxComponent* FrictionPuzzleTriggerBox;
	//
	//UPROPERTY(EditAnywhere)
	//UBoxComponent* LavaPuzzleTriggerBox;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	
};
