// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Camera.generated.h"

class UStaticMeshComponent;
class USpotLightComponent;
class UBoxComponent;

UCLASS()
class GDP_PROJECT_API ACamera : public AActor
{
	GENERATED_BODY()

	
	
public:	
	// Sets default values for this actor's properties
	ACamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	USpotLightComponent* SpotLightComponent;

	UPROPERTY(EditAnywhere)
	UBoxComponent* TriggerBox;

	FTimerHandle WaitTimer;
	FTimerHandle RotationTimer;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetIsActive(bool Value);
	bool GetIsActive() { return bIsActive; }

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);


private:

	enum Direction {
		CLOCKWISE,
		COUNTER_CLOCKWISE
	};

	Direction eDirection;

	FRotator rOriginalRotation;

	void Wait();
	void Rotate();

	bool bIsActive;
	bool bIsRotating;

	int iWaitTime;
	int iRotateTime;

	
	
};
