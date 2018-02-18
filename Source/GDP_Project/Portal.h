// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

class UWidgetComponent;
class UBoxComponent;
class USceneComponent;

UENUM()
enum PuzzleName {
	CAMERA_PUZZLE       UMETA(DisplayName = "Camera/Stealth Puzzle"),
	TIMER_PUZZLE		UMETA(DisplayName = "Timer Puzzle"),
	FRICTION_PUZZLE		UMETA(DisplayName = "Friction Puzzle"),
	LAVA_PUZZLE			UMETA(DisplayName = "Lava Puzzle")
};

UCLASS()
class GDP_PROJECT_API APortal : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Widgets for testing */
	UPROPERTY(EditAnywhere)
	UWidgetComponent* PortalStartWidget;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* PortalEndWidget;

	UPROPERTY(EditAnywhere)
	USceneComponent* PortalEndPoint;

	UPROPERTY(EditAnywhere)
	UBoxComponent*  PortalTriggerBox;

	UPROPERTY(EditAnywhere, Category = "Name")
	TEnumAsByte<PuzzleName> _PuzzleName;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);



};
