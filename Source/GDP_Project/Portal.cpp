// Fill out your copyright notice in the Description page of Project Settings.

#include "Portal.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/LevelScriptActor.h"
#include "OutputDeviceNull.h"
#include "Macros.h"
#include "ToyCar.h"
#include "PossessableActorComponent.h"

// Sets default values
APortal::APortal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	//Trigger Box for activating this spawn point
	PortalTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	PortalTriggerBox->SetupAttachment(RootComponent);
	PortalTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnBeginOverlap);
	PortalTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	PortalEndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndPointComponent"));
	PortalEndPoint->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APortal::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!OtherActor->FindComponentByClass<UPossessableActorComponent>())
		return;

	FOutputDeviceNull ar;

	// This switch can be removed if anyone can figure out how to pass an argument using FOutputDevice!
	switch (_PuzzleName)
	{
		case WARP_ROOM:
			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("ShowWarpRoom"), ar, NULL, true);
			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("HideLounge"), ar, NULL, true);
			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("HidePlayroom"), ar, NULL, true);
			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("HideKitchen"), ar, NULL, true);
			break;

		case LOUNGE:
			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("ShowLounge"), ar, NULL, true);
			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("HideWarpRoom"), ar, NULL, true);
			break;

		case PLAYROOM:
			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("ShowPlayroom"), ar, NULL, true);
			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("HideWarpRoom"), ar, NULL, true);
			break;


		case KITCHEN:
			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("ShowKitchen"), ar, NULL, true);
			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("HideWarpRoom"), ar, NULL, true);
			break;
	}

	OtherActor->SetActorLocationAndRotation(PortalEndPoint->GetComponentLocation(), FRotator(0.0f, 0.0f, 0.0f), false, NULL, ETeleportType::TeleportPhysics);

	AToyCar* pToyCar(Cast<AToyCar>(OtherActor));
	if (pToyCar)
	{
		Cast<AToyCar>(OtherActor)->ResetVelocity();
	}
}
