// Fill out your copyright notice in the Description page of Project Settings.

#include "Portal.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/LevelScriptActor.h"
#include "OutputDeviceNull.h"
#include "Macros.h"

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

	//For testing
	PortalStartWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("RespawnWidgetComponent"));
	PortalStartWidget->SetupAttachment(PortalTriggerBox);
	PortalStartWidget->SetRelativeLocation(FVector(0, 0, 150.0f));
	PortalStartWidget->SetTwoSided(true);

	//For testing
	PortalEndWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("RespawnWidgetComponent2"));
	PortalEndWidget->SetupAttachment(PortalEndPoint);
	static ConstructorHelpers::FClassFinder<UUserWidget> AWidget(TEXT("/Game/TestingPurpose/RespawnSpot"));
	PortalEndWidget->SetWidgetClass(AWidget.Class);
	PortalEndWidget->SetRelativeLocation(FVector(0, 0, 150.0f));
	PortalEndWidget->SetTwoSided(true);
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

	//For the widget testing
	FVector PlayerLoc = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), PlayerLoc);
	PortalStartWidget->SetRelativeRotation(PlayerRot);
	PortalEndWidget->SetRelativeRotation(PlayerRot);
}

void APortal::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	FOutputDeviceNull ar;

	// This switch can be removed if anyone can figure out how to pass an argument using FOutputDevice!
	switch (_PuzzleName)
	{
		case CAMERA_PUZZLE:

			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("ShowCameraPuzzleLevel"), ar, NULL, true);
			break;

		case TIMER_PUZZLE:

			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("ShowTimerPuzzleLevel"), ar, NULL, true);
			break;

		case FRICTION_PUZZLE:

			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("ShowFrictionPuzzleLevel"), ar, NULL, true);
			break;

		case LAVA_PUZZLE:

			GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("ShowLavaPuzzleLevel"), ar, NULL, true);
			break;
	}

	OtherActor->SetActorLocationAndRotation(PortalEndPoint->GetComponentLocation(), FRotator(0.0f, 0.0f, 0.0f), false, NULL, ETeleportType::TeleportPhysics);
}
