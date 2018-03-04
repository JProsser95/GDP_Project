// Fill out your copyright notice in the Description page of Project Settings.

#include "ReturnPortal.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "PossessableActorComponent.h"
#include "Engine/LevelScriptActor.h"
#include "ToyCar.h"
#include "OutputDeviceNull.h"



// Sets default values
AReturnPortal::AReturnPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	WarpRoomReturnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ReturnPoint"));
	WarpRoomReturnPoint->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AReturnPortal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AReturnPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AReturnPortal::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!OtherActor->FindComponentByClass<UPossessableActorComponent>())
		return;

	FOutputDeviceNull ar;

	GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("ShowWarpRoom"), ar, NULL, true);

	OtherActor->SetActorLocationAndRotation(WarpRoomReturnPoint->GetComponentLocation(), FRotator(0.0f, 0.0f, 0.0f), false, NULL, ETeleportType::TeleportPhysics);

	GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("HideAllLevels"), ar, NULL, true);

	Cast<AToyCar>(OtherActor)->ResetVelocity();
}