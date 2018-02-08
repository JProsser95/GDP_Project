// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "ToyTrain.generated.h"

class AToyCar;

UCLASS()
class GDP_PROJECT_API AToyTrain : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AToyTrain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void UpdateCarriages();
	void CompleteTrainPuzzle();

	bool isActive;
	APawn* ToyCar;
	int splinePointer; //this counter is incremented in the Tick() function to move us to the next point on the spline

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called when the Pawn is possesed
	virtual void Restart() override;

	// Allows the addition of a static mesh componenet in the editor
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	bool GetIsActive() { return isActive; }
	void SetIsActive(bool Value);

	void SetToyCar(APawn* TC);

	/** Handle pressing forwards */
	void MoveForward(float fValue);	
	// Posses other Actor;
	void ChangePossesion();

protected:

	UPROPERTY(EditAnywhere)
	USpringArmComponent* OurCameraSpringArm;
	UCameraComponent* OurCamera;

	UPROPERTY(EditAnywhere)
	AActor* Obstacle;

	UPROPERTY(EditAnywhere)
	AActor* TrainHouse;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> Carriages;
};
