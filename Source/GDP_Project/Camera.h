// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Camera.generated.h"

class UStaticMeshComponent;
class USpotLightComponent;
class UBoxComponent;

UENUM()
enum CameraType {
	ROTATING_CAMERA     UMETA(DisplayName = "Rotating Camera"),
	STATIC_CAMERA		UMETA(DisplayName = "Static Camera"),
	FLICKERING_CAMERA   UMETA(DisplayName = "Flickering Camera")
};

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
	FTimerHandle ActionTimer;

	UPROPERTY(EditAnywhere, Category = "Type of Camera")
	TEnumAsByte<CameraType> _CameraType;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetIsActive(bool Value);
	bool GetIsActive() { return bIsActive; }

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	enum Direction {
		CLOCKWISE,
		COUNTER_CLOCKWISE
	};

	Direction eDirection;

	FRotator rOriginalRotation;

	void Wait();
	void Action();
	void CameraHit();

	bool bIsActive;
	bool bIsRotating;
	bool bIsShining;
	bool bIsSafe;

	int iWaitTime;
	int iActionTime;

	
	
};
