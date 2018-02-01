// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RespawnPoint.generated.h"

class UWidgetComponent;
class UBoxComponent;

UCLASS()
class GDP_PROJECT_API ARespawnPoint : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ARespawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Widgets for testing */
	UPROPERTY(EditAnywhere)
	UWidgetComponent* RespawnWidget;
	UPROPERTY(EditAnywhere)
	UWidgetComponent* TriggerWidget;

	UPROPERTY(EditAnywhere)
	UBoxComponent* TriggerBox;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool GetIsCurrentSpawnPoint() { return isCurrentSpawnPoint; }

	/* Sets the spawn point
	* @param value Sets isCurrentSpawnPoint to value
	*/
	void SetisCurrentSpawnPoint(bool value);

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);


private:

	bool isCurrentSpawnPoint;
	
};
