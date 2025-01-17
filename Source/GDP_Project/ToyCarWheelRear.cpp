// Fill out your copyright notice in the Description page of Project Settings.

#include "ToyCarWheelRear.h"
#include "TireConfig.h"
#include "UObject/ConstructorHelpers.h"


UToyCarWheelRear::UToyCarWheelRear()
{
	ShapeRadius = 18.0f;
	ShapeWidth = 15.0f;
	bAffectedByHandbrake = true;
	SteerAngle = 0.f;

	// Setup suspension forces
	//SuspensionForceOffset = -4.0f;
	//SuspensionMaxRaise = 8.0f;
	SuspensionMaxDrop = 4.0f; // Previously 12.0f and commented out...
	//SuspensionNaturalFrequency = 9.0f;
	//SuspensionDampingRatio = 1.05f;

	LatStiffValue = 100.0f;

	// Find the tire object and set the data for it
	static ConstructorHelpers::FObjectFinder<UTireConfig> TireData(TEXT("TireConfig'/Game/VehicleAdv/Vehicle/WheelData/Vehicle_BackTireConfig.Vehicle_BackTireConfig'"));
	TireConfig = TireData.Object;
}

void UToyCarWheelRear::ModifyFriction(float fNewFriction)
{
	LatStiffValue = fNewFriction;
}

bool UToyCarWheelRear::InAir()
{
	return IsInAir();
}
