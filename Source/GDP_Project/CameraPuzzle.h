// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraPuzzle.generated.h"

class UWidgetComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UDirectionalLightComponent;
class UCameraComponent;
class ACameraDirector;
class ACamera;
class AToyCar;

UCLASS()
class GDP_PROJECT_API ACameraPuzzle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraPuzzle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Safe;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* TriggerWidget;

	UPROPERTY(EditAnywhere)
	UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere)
	UDirectionalLightComponent* DirectionalLight;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	FTimerHandle SafeTimer;

	ACameraDirector* CameraDirector;

	AToyCar* Car;

	void OpenSafe();
	void CloseSafe();


private:

	TArray<ACamera*> Cameras;

	bool bIsActive;
	bool bPuzzleFailed;
	bool bIsClosingSafe;
	bool bIsOpeningSafe;
	int iBrighter;
	int iSafeTime;


	float fLightIntensity;
	float fOpenSafeHeight;
	float fClosedSafeHeight;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void PuzzleFailed();

	void SetIsActive(bool Value) { bIsActive = Value; }
	bool GetIsActive() { return bIsActive; }

};
