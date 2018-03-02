// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FrictionPuzzle.generated.h"

class UBoxComponent;
class UCameraComponent;
class ACameraDirector;

UCLASS()
class GDP_PROJECT_API AFrictionPuzzle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFrictionPuzzle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UBoxComponent* StickyTriggerBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* CameraEventTriggerBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StickyFloor;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	ACameraDirector* CameraDirector;

	FTimerHandle CameraTimer;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	bool bIsCarStuck;
	bool bCameraChanged;

	int iCameraTime;

	void ChangeCamera();
	void CleanSticky();
};
