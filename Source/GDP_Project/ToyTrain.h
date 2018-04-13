// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "PlanePart.h"

#include "ToyTrain.generated.h"

#define NUMBEROFTRACKSWITCHERS 4 // Used for the car to weigh down

#define STATIONWAITTIME 3.0f

enum TRAIN_STATES
{
	RunawayTrain,
	RunawayTrain_Failed,

	RunawayTrain2,
	RunawayTrain2_Station,
	RunawayTrain2_Failed,

	RunawayTrain3,

	RunawayTrain4,

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
	
	int splinePointer; //this counter is incremented in the Tick() function to move us to the next point on the spline
	TArray<TArray<FVector>> pathPointLocation;//save sampled point locations into an array
	TArray<TArray<FQuat>> pathPointRotation;//save sampled point rotations into an array

	int MovementDirection;
	TRAIN_STATES TrainState;		// Current state of the train. Used to swap between train lines.

	float m_fStationWaitTime;
	bool m_bCarriageAttached;
	bool m_bRotating;

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


	// Trigger functions that can be called from other classes
	void TrackSwitcherHit(int TrackSwitchNumber);

	// Accessors
	int GetSplineCounter() { return splinePointer; }
	TRAIN_STATES GetTrainState() { return TrainState; }

	bool OnFailureTrainLine();
	bool OnCompletionTrainLine();

	bool TrainPuzzleFailed();
	bool TrainPuzzleCompleted();

	// Mutators
	void SetSplineCounter(int iSplineCounter) { splinePointer = iSplineCounter; }
	void SetTrainState(TRAIN_STATES eTrainState) { TrainState = eTrainState; }
	void SetMovementDirection(int iDirection) { MovementDirection = iDirection; }

protected:
	// Allows the addition of a static mesh componenet in the editor
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* OurCameraSpringArm;
	UCameraComponent* OurCamera;

	UPROPERTY(Category = TrackSwapping, EditAnywhere)
	UBoxComponent* StartRaceTriggerBox;

	UPROPERTY(Category = TrackSwapping, EditAnywhere)
	UBoxComponent* StopWaitingAtStationTriggerBox;

	UPROPERTY(Category = TrackSwapping, EditAnywhere)
	TArray<AActor*> SplineBPs;

	UPROPERTY(Category = TrackPieces, EditAnywhere)
	AActor* RotatingTrack;

	UPROPERTY(Category = PuzzlePieces, EditAnywhere)
	TArray<AActor*> Carriages;

	UPROPERTY(Category = PuzzlePieces, EditAnywhere)
	APlanePart* PlanePart;
};
