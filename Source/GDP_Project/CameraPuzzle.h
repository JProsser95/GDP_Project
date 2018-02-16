// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraPuzzle.generated.h"

class UWidgetComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UDirectionalLightComponent;
class ACamera;

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
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* TriggerWidget;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* CompleteWidget;

	UPROPERTY(EditAnywhere)
	UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere)
	UDirectionalLightComponent* DirectionalLight;

private:

	TArray<ACamera*> Cameras;

	bool bIsActive;
	bool bPuzzleFailed;
	int iBrighter;

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
