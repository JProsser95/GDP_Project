// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "ToyTrain.generated.h"


#define NUMBEROFTRACKSWITCHERS 2

enum TRAIN_STATES
{
	RunawayTrain,
	RunawayTrain_Failed,

	RunawayTrain2,
	RunawayTrain2_Failed,

	RunawayTrain3,

	TRAIN_STATES_MAX
};

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
	void UpdateState();
	void ChangeToState(TRAIN_STATES newState);

	bool AutomatedMovement(); // returns false if the train can't move

	void UpdateSplinePointer();
	void UpdateTrainOnSpline();
	void UpdateCarriages();
	void CompleteTrainPuzzle();

	bool isActive;
	APawn* ToyCar;
	
	int splinePointer; //this counter is incremented in the Tick() function to move us to the next point on the spline
	TArray<TArray<FVector>> pathPointLocation;//save sampled point locations into an array
	TArray<TArray<FQuat>> pathPointRotation;//save sampled point rotations into an array
	
	bool Rotating;

	int MovementDirection;
	TRAIN_STATES TrainState;

	// Variables used in each train state
	// Runaway train
	bool TrackSwitched[NUMBEROFTRACKSWITCHERS]; // Has the car pressed the track switcher in time?
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called when the Pawn is possesed
	virtual void Restart() override;

	bool GetIsActive() { return isActive; }
	void SetIsActive(bool Value);

	void SetToyCar(APawn* TC);

	/** Handle pressing forwards */
	void MoveForward(float fValue);	
	// Posses other Actor;
	void ChangePossesion();


	// Trigger functions that can be called from other classes
	void TrackSwitcherHit(int TrackSwitchNumber);

protected:
	// Allows the addition of a static mesh componenet in the editor
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* OurCameraSpringArm;
	UCameraComponent* OurCamera;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> SplineBPs;

	UPROPERTY(EditAnywhere)
	AActor* Obstacle;

	UPROPERTY(EditAnywhere)
	AActor* TrainHouse;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> Carriages;
};
