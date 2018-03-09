// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CustomMovementComponent.generated.h"

UCLASS()
class GDP_PROJECT_API UCustomMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

	UCustomMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	bool m_bHitObject;
public:
	bool HitObject();

};
