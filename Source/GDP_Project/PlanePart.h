// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlanePart.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UENUM()
enum Part {
	PROPELLER			UMETA(DisplayName = "Propeller"),
	HULL				UMETA(DisplayName = "Hull"),
	LEFT_WING			UMETA(DisplayName = "Left Wing"),
	RIGHT_WING			UMETA(DisplayName = "Right Wing"),
	WHEELS				UMETA(DisplayName = "Wheels"),
	MAX
};

UCLASS()
class GDP_PROJECT_API APlanePart : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlanePart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere)
	UBoxComponent*  TriggerBox;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//void SetIsCollected(bool Value) { bIsCollected = Value; }
	//bool GetIsCollected() { return bIsCollected; }

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	Part _PartName;

private:

	void CollectPart();
	
};