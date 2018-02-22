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

/**
 * 
 */
UCLASS()
class GDP_PROJECT_API AToyCar : public AWheeledVehicle
{
	GENERATED_BODY()
	
	enum Sounds {
		ENGINE = 0,
		BREAK  = 1
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
	float fLastUnFocusTime;

	UPROPERTY(Category = Reset, EditAnywhere)
	float RespawnDelay;
	float fLastRespawn;

	/** Are we in reverse gear */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	bool bInReverseGear;

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

	// Resets the camera to the be directly behind the car
	void UpdateCamera(float DeltaTime);

	void LimitCarRotation();

	bool GetIsActive() { return isActive; }
	void SetIsActive(bool Value);

	bool GetCanMove() { return bCanMove; }
	void SetCanMove(bool Value) { bCanMove = Value; }

	// For the Friction Puzzle
	void OnSticky();
	void OffSticky();

	static const FName LookUpBinding;
	static const FName LookRightBinding;
	static const FName EngineAudioRPM;

	UPROPERTY(Category = Collider, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CameraParent;

	UPROPERTY(EditAnywhere)
	APossessionChangerManager* PossessionChangerManager;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(Category = Sound, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* AudioComponent;

private:

	Sounds currentSoundCue;

	TArray<USoundCue*> AudioCues;

	/* Are we on a 'slippery' surface */
	bool bIsLowFriction;
	/** Slippery Material instance */
	UPhysicalMaterial* SlipperyMaterial;
	/** Non Slippery Material instance */
	UPhysicalMaterial* NonSlipperyMaterial;

	//Is the car able to currently posses another Pawn
	bool bCanPosses;
	bool isActive;
	bool isBreaking;
	//Set this to false if we don't want the car to be able to move
	bool bCanMove;

	// For the friction puzzle
	float fSitckyFriction;

	APawn* possesActor;

	FVector2D CameraInput;

public:
	/** Returns SpringArm subobject **/
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
};
