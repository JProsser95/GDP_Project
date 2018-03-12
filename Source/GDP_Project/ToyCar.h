// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "PossessionChangerManager.h"
#include "ToyCar.generated.h"

class UPhysicalMaterial;
class UCameraComponent;
class USpringArmComponent;
class USphereComponent;
class UWidgetComponent;
class UPossessableActorComponent;
class USoundCue;
class UAudioComponent;

UENUM()
enum PuzzleName {
	TIMER				UMETA(DisplayName = "Timer"),
	FRICTION			UMETA(DisplayName = "Friction"),
	TRAIN				UMETA(DisplayName = "Plane"),
};

/**
 * 
 */
UCLASS()
class GDP_PROJECT_API AToyCar : public AWheeledVehicle
{
	GENERATED_BODY()
	
	enum Sounds {
		ENGINE = 0,
		BRAKE  = 1
	};

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPossessableActorComponent* PossessableComponent;

public:

	AToyCar();

	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator CameraRotation;
	FRotator CameraRotationOffset;

	UPROPERTY(Category = Camera, EditAnywhere)
	bool AutoFocus;

	UPROPERTY(Category = Camera, EditAnywhere)
	float AutoFocusDelay;
	float m_fLastUnFocusTime;

	UPROPERTY(Category = Reset, EditAnywhere)
	float RespawnDelay;
	float m_fLastRespawn;

	/** Are we in reverse gear */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	bool m_bInReverseGear;

	// Begin Pawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End Pawn interface

	// Begin Actor interface
	virtual void Tick(float Delta) override;

	// Called when the Pawn is possesed
	virtual void Restart() override;

	UPROPERTY(Category = Rotation, EditAnywhere)
	float MaxAngle;

	UPROPERTY(Category = Rotation, EditAnywhere)
	float RotateSpeed;

	UPROPERTY(Category = Rotation, EditAnywhere)
	bool LimitRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Hints")
	bool CanSeeHints;

	float m_fTurnAmount;
	float m_fTimeOnGround;

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void UpdatePlanePartsInHUD(int PlanePartNumber);

	UPROPERTY(BlueprintReadOnly)
	bool TimerPuzzleCompleted;

	UPROPERTY(BlueprintReadOnly)
	bool FrictionPuzzleCompleted;

	UPROPERTY(BlueprintReadOnly)
	bool TrainPuzzleCompleted;

	void SetPuzzleCompleted(PuzzleName Name);

protected:
	virtual void BeginPlay() override;

public:
	// End Actor interface

	/** Handle pressing forwards */
	void MoveForward(float Val);

	/** Update the physics material used by the vehicle mesh */
	void UpdatePhysicsMaterial();

	/** Handle pressing right */
	void MoveRight(float Val);
	/** Handle handbrake pressed */
	void OnHandbrakePressed();
	/** Handle handbrake released */
	void OnHandbrakeReleased();
	// Posses other Actor;
	void ChangePossesion();
	// Either bring up or remove the HUD
	void ChangeHUD();

	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);

	// Flips the car back to being flat (Used when the car is flipped over)
	void ResetPositionAndRotation();

	// Respawns the car at the current active safe spot
	void Respawn();

	// Resets the cars velocity and angular velocity to zero
	UFUNCTION(BlueprintCallable)
	void ResetVelocity();

	// Resets the camera to the be directly behind the car
	void UpdateCamera(float DeltaTime);

	void LimitCarRotation(float DeltaTime);

	bool GetCanMove() { return m_bCanMove; }
	UFUNCTION(BlueprintCallable, Category = "CarMovement")
	void SetCanMove(bool Value) { m_bCanMove = Value; }

	bool GetIsInPuzzle() { return m_bIsInPuzzle; }
	void SetIsInPuzzle(bool Value) { m_bIsInPuzzle = Value; CanSeeHints = false; }

	void SetPossessionChangeManager(APossessionChangerManager* PCM) { PossessionChangerManager = PCM; }

	// For the Friction Puzzle
	void OnSticky();
	void OffSticky();

	UPROPERTY(Category = Collider, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CameraParent;

	UPROPERTY(EditAnywhere)
	APossessionChangerManager* PossessionChangerManager;

	UPROPERTY(Category = Sound, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* AudioComponent;

	bool InAir();
	void SetLatStiff(float fNewLatStiff);
private:

	Sounds currentSoundCue;

	TArray<USoundCue*> AudioCues;

	/* Are we on a 'slippery' surface */
	bool m_bIsLowFriction;
	/** Slippery Material instance */
	UPhysicalMaterial* SlipperyMaterial;
	/** Non Slippery Material instance */
	UPhysicalMaterial* NonSlipperyMaterial;

	//Is the car able to currently posses another Pawn
	bool m_bIsBraking;
	//Set this to false if we don't want the car to be able to move
	bool m_bCanMove;

	bool m_bIsInPuzzle;

	// For the friction puzzle
	float m_fStickyFriction;

	FVector2D CameraInput;

public:
	/** Returns SpringArm subobject **/
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
};
