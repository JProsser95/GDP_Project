// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomMovementComponent.h"

UCustomMovementComponent::UCustomMovementComponent()
	:m_bHitObject(false)
{

}

void UCustomMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Make sure that everything is still valid, and that we are allowed to move.
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	// Get (and then clear) the movement vector that we set in APawnTesting::Tick
	FVector DesiredMovementThisFrame = ConsumeInputVector() * DeltaTime;
	FVector StartPos = UpdatedComponent->RelativeLocation;

	if (!DesiredMovementThisFrame.IsNearlyZero())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

		// If we bumped into something, try to slide along it
		if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(DesiredMovementThisFrame, 1.0f - Hit.Time, Hit.Normal, Hit);
			if ((UpdatedComponent->RelativeLocation - StartPos).Size() <= (DesiredMovementThisFrame*0.8f).Size())
				m_bHitObject = true;
			else
				m_bHitObject = false;
		}
		else
			m_bHitObject = false;
	}
};

bool UCustomMovementComponent::HitObject()
{
	return m_bHitObject;
}
