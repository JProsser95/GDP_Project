// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Engine/EngineTypes.h"
#include "TimePuzzle.generated.h"

class UWidgetComponent;
class UBoxComponent;
class UStaticMeshComponent;
class UCameraComponent;
class ACameraDirector;
class AToyCar;

UCLASS()
class GDP_PROJECT_API ATimePuzzle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATimePuzzle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* TriggerWidget;

	UPROPERTY(EditAnywhere)
	UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Door;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	FTimerHandle DoorTimer;

	ACameraDirector* CameraDirector;

	AToyCar* Car;

	void OpenDoor();
	void PuzzleComplete();

	UPROPERTY(EditAnywhere)
	TArray<AActor*> Actors;

	TArray<AActor*> CopyActors;

	bool AllPointsCollected();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
private:

	int GetVisibleActors();
	void PointManage(float DeltaTime);
	void PuzzleFailed();

	bool bIsPuzzleTriggered;
	bool bIsPuzzleComplete;
	bool bIsOpeningDoor;

	int iDoorTime;

	float RingSmallScale;
};
