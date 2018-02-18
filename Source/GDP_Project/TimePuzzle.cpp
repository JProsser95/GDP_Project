// Fill out your copyright notice in the Description page of Project Settings.

#include "TimePuzzle.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SceneComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "CameraDirector.h"
#include "ToyCar.h"
#include "GDP_ProjectGameModeBase.h"
#include "EngineUtils.h"
#include "Macros.h"

const int MAX_DOOR_TIMER(5);

// Sets default values
ATimePuzzle::ATimePuzzle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	//For testing
	TriggerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("RespawnWidgetComponent"));
	TriggerWidget->SetupAttachment(RootComponent);
	static ConstructorHelpers::FClassFinder<UUserWidget> TWidget(TEXT("/Game/TestingPurpose/TimePuzzle"));
	TriggerWidget->SetWidgetClass(TWidget.Class);
	TriggerWidget->SetRelativeLocation(FVector(0, 0, 150.0f));
	TriggerWidget->SetTwoSided(true);

	//For testing
	CompleteWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CompleteWidgetComponent"));
	CompleteWidget->SetupAttachment(RootComponent);
	static ConstructorHelpers::FClassFinder<UUserWidget> CWidget(TEXT("/Game/TestingPurpose/PuzzleComplete"));
	CompleteWidget->SetWidgetClass(CWidget.Class);
	CompleteWidget->SetRelativeLocation(FVector(0, 0, 150.0f));
	CompleteWidget->SetTwoSided(true);

	//Trigger Box for activating this spawn point
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATimePuzzle::OnBeginOverlap);
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorComponent"));
	Door->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	bIsPuzzleTriggered = false;
	bIsClosingDoor = false;
	bIsOpeningDoor = false;
	iDoorTime = MAX_DOOR_TIMER;
}

// Called when the game starts or when spawned
void ATimePuzzle::BeginPlay()
{
	Super::BeginPlay();
	
	// Get the Camera Director that is in the scene
	for (TActorIterator<ACameraDirector> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		CameraDirector = *ActorItr;
	}
}

// Called every frame
void ATimePuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPuzzleTriggered && !bIsClosingDoor && !bIsOpeningDoor) 
	{
		AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
		if (GameMode != nullptr)
		{
			if (GameMode->GetTimeLeft() <= 0)
			{
				bIsPuzzleTriggered = false;
				GameMode->RemoveTimerWidget();
				bIsClosingDoor = true;
				iDoorTime = MAX_DOOR_TIMER;
				GetWorldTimerManager().SetTimer(DoorTimer, this, &ATimePuzzle::CloseDoor, 1.0f, true, 0.0f);
			}
		}
	}

	if (bIsClosingDoor)
	{
		FVector DoorLocation = Door->GetComponentLocation();
		Door->SetWorldLocation(DoorLocation - FVector(0, 0, DeltaTime * 100));
	}

	if (bIsOpeningDoor)
	{
		FVector DoorLocation = Door->GetComponentLocation();
		Door->SetWorldLocation(DoorLocation + FVector(0, 0, DeltaTime * 100));
	}

	//For the widget testing
	FVector PlayerLoc = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), PlayerLoc);
	TriggerWidget->SetRelativeRotation(PlayerRot);
	CompleteWidget->SetRelativeRotation(PlayerRot);
}

void ATimePuzzle::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (bIsPuzzleTriggered)
		return;

	bIsPuzzleTriggered = true;
	bIsOpeningDoor = true;

	if (CameraDirector != nullptr)
		CameraDirector->BeginTimePuzzleCameraChange(OtherActor);

	Car = Cast<AToyCar>(OtherActor);

	if (Car != nullptr)
		Car->SetCanMove(false);

	iDoorTime = MAX_DOOR_TIMER;
	GetWorldTimerManager().SetTimer(DoorTimer, this, &ATimePuzzle::OpenDoor, 1.0f, true, 0.0f);
}

void ATimePuzzle::CloseDoor()
{
	if (--iDoorTime <= 0)
	{
		bIsClosingDoor = false;
		iDoorTime = MAX_DOOR_TIMER;
		GetWorldTimerManager().ClearTimer(DoorTimer);
	}
}

void ATimePuzzle::OpenDoor()
{
	if (--iDoorTime <= 0)
	{
		bIsOpeningDoor = false;
		iDoorTime = MAX_DOOR_TIMER;
		if (CameraDirector != nullptr)
			CameraDirector->BeginTimePuzzleCameraChange();
		if (Car != nullptr)
			Car->SetCanMove(true);
		GetWorldTimerManager().ClearTimer(DoorTimer);
	}
}