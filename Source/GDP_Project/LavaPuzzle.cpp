// Fill out your copyright notice in the Description page of Project Settings.

#include "LavaPuzzle.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "EngineUtils.h"
#include "RespawnPoint.h"
#include "ToyCar.h"
#include "Macros.h"

// Sets default values
ALavaPuzzle::ALavaPuzzle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	//Trigger Box for activating this spawn point
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALavaPuzzle::OnBeginOverlap);
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));
}

// Called when the game starts or when spawned
void ALavaPuzzle::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALavaPuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALavaPuzzle::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	for (TActorIterator<ARespawnPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetIsCurrentSpawnPoint())
		{

			FVector spawnLocation = ActorItr->GetActorLocation();
			spawnLocation.Z += 100;
			OtherActor->SetActorLocationAndRotation(spawnLocation, FRotator(0, 0, 0), false, NULL, ETeleportType::TeleportPhysics);
			Cast<AToyCar>(OtherActor)->ResetVelocity();
		}
	}
}