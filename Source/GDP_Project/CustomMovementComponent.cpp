// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomMovementComponent.h"
#include "Runtime/Engine/Classes/Components/PrimitiveComponent.h"
//#include "Runtime/Engine/Classes/Components/DestructibleComponent.h"

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

	m_bHitObject = false;
	m_bHitWindow = false;

	if (!DesiredMovementThisFrame.IsNearlyZero())
	{
		FHitResult Hit;

		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

		// If we bumped into something, try to slide along it
		if (Hit.IsValidBlockingHit())
		{
			if (Hit.Component != nullptr)
			{
				m_bHitObject = false;

				if (Hit.Component->ComponentHasTag("Window"))
				{
					m_bHitWindow = true;
					//Hit.Component
					//UDestructibleComponent* test = Hit.Component->GetDestructibleComponent();
				}
				//Hit.Component->AddImpulse
				SlideAlongSurface(DesiredMovementThisFrame, 1.0f - Hit.Time, Hit.Normal, Hit);
				if ((UpdatedComponent->RelativeLocation - StartPos).Size() <= (DesiredMovementThisFrame*0.8f).Size())
					m_bHitObject = !m_bHitWindow;
				else
					m_bHitObject = false;
			}
		}
	}
};

bool UCustomMovementComponent::HitObject()
{
	return m_bHitObject;
}

bool UCustomMovementComponent::HitWindow()
{
	return m_bHitWindow;
}
