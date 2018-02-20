// Fill out your copyright notice in the Description page of Project Settings.

#include "ToyCarWheelFront.h"
#include "TireConfig.h"
#include "UObject/ConstructorHelpers.h"


UToyCarWheelFront::UToyCarWheelFront()
{
	ShapeRadius = 18.f;
	ShapeWidth = 15.0f;
	bAffectedByHandbrake = false;
	SteerAngle = 40.f;

	// Setup suspension forces
	//SuspensionForceOffset = -4.0f;
	//SuspensionMaxRaise = 8.0f;
	SuspensionMaxDrop = 2.0f; // Previously 12.0f and commented out...
	//SuspensionNaturalFrequency = 9.0f;
	//SuspensionDampingRatio = 1.05f;

	LatStiffValue = 100.0f;

	// Find the tire object and set the data for it
	static ConstructorHelpers::FObjectFinder<UTireConfig> TireData(TEXT("TireConfig'/Game/VehicleAdv/Vehicle/WheelData/Vehicle_FrontTireConfig.Vehicle_FrontTireConfig'"));
	TireConfig = TireData.Object;
}

void UToyCarWheelFront::ModifyFriction(float fNewFriction)
{
	LatStiffValue = fNewFriction;
}