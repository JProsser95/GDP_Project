// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShake.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "ToyPlane.generated.h"

class UPossessableActorComponent;

UCLASS()
class GDP_PROJECT_API AToyPlane : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AToyPlane();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPossessableActorComponent* possComponent;

	enum CameraType {
		FIRST_PERSON,
		THIRD_PERSON
	};

	// Players initial boost
	UPROPERTY(EditAnywhere, Category = "Plane")
	float MaximumBoost;

	// Players current boost
	UPROPERTY(VisibleAnywhere, Category = "Plane")
	float CurrentBoost;

	// Is the plane currently boosting
	UPROPERTY(EditAnywhere, Category = "Plane")
	bool IsBoosting;

	UPROPERTY(EditAnywhere, Category = "Plane")
	float MinSpeed;

	UPROPERTY(EditAnywhere, Category = "Plane")
	float MaxSpeed;

	UPROPERTY(EditAnywhere, Category = "Plane")
	float CamShakeSpeed;

	UPROPERTY(EditAnywhere, Category = "Plane")
	float SpeedIncrement;

	UPROPERTY(EditAnywhere, Category = "Plane")
	float BoostSpeedIncrement;

	UPROPERTY(EditAnywhere, Category = "Plane")
	float RotateSpeed;

	UPROPERTY(EditAnywhere, Category = "Plane")
	float TurnSpeed;

	UPROPERTY(EditAnywhere, Category = "Plane")
	float PropRotateSpeed;

	UPROPERTY(EditAnywhere, Category = "Plane")
	bool bIsActive;

	float fPropRotation;
	float fSpeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called when the Pawn is possesed
	virtual void Restart() override;

	// Accessor for MaximumBoost
	UFUNCTION(BlueprintPure, Category = "Plane")
	float GetMaximumBoost() { return MaximumBoost; }

	// Accessor for CurrentBoost
	UFUNCTION(BlueprintPure, Category = "Plane")
	float GetCurrentBoost() { return CurrentBoost; }

	// Accessor for isBoosting
	UFUNCTION(BlueprintPure, Category = "Plane")
	bool GetIsBoosting() { return IsBoosting; }

	/* Updates current boost
	* @param Boost The amount the boost of the plane will change 
	*/
	UFUNCTION(BluePrintCallable, Category = "Plane")
	void UpdateCurrentBoost(float boostIncrement);

	class UCustomMovementComponent* CustomMovementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	void SetIsActive(bool Value);
	bool GetIsActive() { return bIsActive; }

protected:

	// The buleprint for the camera shake 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Plane")
	TSubclassOf<UCameraShake> CameraShake;

	// Widget class to use for HUD screen
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Plane", Meta = (BlueprintProtected = true))
	TSubclassOf<class UUSerWidget> HUDWidgetClass;

	// Instance of the HUD
	UPROPERTY()
	class UUserWidget* CurrentWidget;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* OurCameraSpringArm;
	UCameraComponent* OurCamera;

	// Allows the addition of a static mesh component in the editor
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PlaneBodyMeshComponent;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PlanePropMeshComponent;

	CameraType eCameraType;
	
	//Input variables
	FVector2D MovementInput;
	FVector2D CameraInput;

	//Collision functions
	UFUNCTION()
	void OnToyPlaneOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//Input functions
	void MoveUp(float AxisValue);
	void MoveRight(float AxisValue);
	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);
	void StartBoost();
	void EndBoost();
	void CameraZoom();
};
