// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleWheel.h"
#include "ToyCarWheelRear.generated.h"

/**
 * 
 */
UCLASS()
class GDP_PROJECT_API UToyCarWheelRear : public UVehicleWheel
{
	GENERATED_BODY()
	
public:
	UToyCarWheelRear();
	void ModifyFriction(float fNewFriction);
	bool InAir();
};
