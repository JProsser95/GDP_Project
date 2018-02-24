// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "TrackSwappingManager.h"
#include "PossessionChangerManager.h"

#include "ToyTrain.generated.h"


#define NUMBEROFTRACKSWITCHERS 2 // Used for the car to weigh down

enum TRAIN_STATES
{
	RunawayTrain,
	RunawayTrain_Failed,

	RunawayTrain2,
	RunawayTrain2_Failed,

	RunawayTrain3,

	PossessableTrain,
	PossessableTrain2,
	PossessableTrain3,
	PossessableTrain4,
	PossessableTrain5,
	PossessableTrain6,

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

	bool StartOfCurrentLine();
	bool EndOfCurrentLine();

	void SetTrainStateToChangeTo(int SwitchActivated);
	
	int splinePointer; //this counter is incremented in the Tick() function to move us to the next point on the spline
	TArray<TArray<FVector>> pathPointLocation;//save sampled point locations into an array
	TArray<TArray<FQuat>> pathPointRotation;//save sampled point rotations into an array
	
	bool Rotating; // Is the train currently rotating?
	bool CarriageAttached; // Has the carriage been attached to the train?

	int MovementDirection;
	TRAIN_STATES TrainState;		// Current state of the train. Used to swap between train lines.

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

	/** Handle pressing forwards */
	void MoveForward(float fValue);	
	// Possess other Actor;
	void ChangePossesion();
	// Swap closest track
	void SwapTrack();


	// Trigger functions that can be called from other classes
	void TrackSwitcherHit(int TrackSwitchNumber);

	// Accessors
	int GetSplineCounter() { return splinePointer; }
	TRAIN_STATES GetTrainState() { return TrainState; }
	bool OnFailureTrainLine();
	bool TrainPuzzleFailed();

	// Mutators
	void SetSplineCounter(int iSplineCounter) { splinePointer = iSplineCounter; }
	void SetTrainState(TRAIN_STATES eTrainState) { TrainState = eTrainState; }

protected:
	// Allows the addition of a static mesh componenet in the editor
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* OurCameraSpringArm;
	UCameraComponent* OurCamera;

	UPROPERTY(EditAnywhere)
	APossessionChangerManager* PossessionChangerManager;

	UPROPERTY(Category = TrackSwapping, EditAnywhere)
	ATrackSwappingManager* TrackSwappingManager;

	UPROPERTY(Category = TrackSwapping, EditAnywhere)
	TArray<AActor*> SplineBPs;

	UPROPERTY(Category = TrackPieces, EditAnywhere)
	AActor* Obstacle;

	UPROPERTY(Category = TrackPieces, EditAnywhere)
	AActor* RotatingTrack;

	UPROPERTY(Category = TrackPieces, EditAnywhere)
	AActor* BridgePieces[2];

	UPROPERTY(Category = PuzzlePieces, EditAnywhere)
	AActor* TrainHouse;

	UPROPERTY(Category = PuzzlePieces, EditAnywhere)
	TArray<AActor*> Carriages;
};
