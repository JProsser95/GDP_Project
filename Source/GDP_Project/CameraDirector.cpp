// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraDirector.h"
#include "GDP_ProjectGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACameraDirector::ACameraDirector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bCameraChange = false;

}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACameraDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACameraDirector::BeginCameraChange()
{
	float SmoothBlendTime = 0.75f;

	//Find the actor that handles control for the local player.
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (OurPlayerController)
	{
		if ((OurPlayerController->GetViewTarget() != CarCamera) && (CarCamera != nullptr))
		{
			//Cut instantly to camera one.
			OurPlayerController->SetViewTarget(CarCamera);
			AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
			GameMode->BeginTimer();
		}
		else if ((OurPlayerController->GetViewTarget() != GarageCamera) && (GarageCamera != nullptr))
		{
			//Blend smoothly to camera two.
			OurPlayerController->SetViewTargetWithBlend(GarageCamera, SmoothBlendTime);
		}
	}
}