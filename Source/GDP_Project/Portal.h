// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ToyCar.h"
#include "Portal.generated.h"

class UWidgetComponent;
class UBoxComponent;
class USceneComponent;




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

	UPROPERTY(EditAnywhere)
	USceneComponent* PortalEndPoint;

	UPROPERTY(EditAnywhere)
	UBoxComponent*  PortalTriggerBox;

	UPROPERTY(EditAnywhere, Category = "Name")
	TEnumAsByte<RoomName> _PuzzleName;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Teleport();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AToyCar* pToyCar;

	UPROPERTY(EditAnywhere)
	int PlanePartsRequired;

};
