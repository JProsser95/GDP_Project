// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterGlass.generated.h"

class UCleanerObject;
class UBoxComponent;
class UStaticMeshComponent;
class UCameraComponent;
class ACameraDirector;

UCLASS()
class GDP_PROJECT_API AWaterGlass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaterGlass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UBoxComponent* GlassTriggerBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* CouchTriggerBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* GlassMesh;

	UCleanerObject* Cleaner;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	ACameraDirector* CameraDirector;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);


private:

	bool bCouchHit;
	
};
