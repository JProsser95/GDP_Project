// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleWheel.h"
#include "ToyCarWheelFront.generated.h"

/**
 * 
 */
UCLASS()
class GDP_PROJECT_API UToyCarWheelFront : public UVehicleWheel
{
	GENERATED_BODY()
	
public:
	UToyCarWheelFront();
	void ModifyFriction(float fNewFriction);
};
