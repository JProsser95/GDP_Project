// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShake.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"


#include "ToyPlane.generated.h"

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

	enum CameraType {
		FIRST_PERSON,
		THIRD_PERSON
	};

	// Players initial boost
	UPROPERTY(EditAnywhere, Category = "Plane")
	float fInitialBoost;

	// Players current boost
	UPROPERTY(EditAnywhere, Category = "Plane")
	float fCurrentBoost;

	// Is the plane currently boosting
	UPROPERTY(EditAnywhere, Category = "Plane")
	bool bIsBoosting;

	float fSpeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called when the Pawn is possesed
	virtual void Restart() override;

	// Accessor for InitialBoost
	UFUNCTION(BlueprintPure, Category = "Plane")
	float GetInitialBoost() { return fInitialBoost; }

	// Accessor for CurrentBoost
	UFUNCTION(BlueprintPure, Category = "Plane")
	float GetCurrentBoost() { return fCurrentBoost; }

	// Accessor for isBoosting
	UFUNCTION(BlueprintPure, Category = "Plane")
	bool GetIsBoosting() { return bIsBoosting; }

	/* Updates current boost
	* @param Boost The amount the boost of the plane will change 
	*/
	UFUNCTION(BluePrintCallable, Category = "Plane")
	void UpdateCurrentBoost(float currentBoost);

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
	UStaticMeshComponent* MeshComponent;

	// Allows the additions of a collider component in the editor
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComponent;

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
