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
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
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

	for (int i = 0; i < Actors.Num(); ++i)
	{
		Actors[i]->SetActorHiddenInGame(true);
	}

	CopyActors = Actors;
}

// Called every frame
void ATimePuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPuzzleTriggered) 
	{
		AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
		if (GameMode != nullptr)
		{
			if (GameMode->GetTimeLeft() <= 0)
			{
				GameMode->RemoveTimerWidget();
				PuzzleFailed();
			}
		}

		if (AllPointsCollected())
		{
			PuzzleComplete();
		}

		PointManage();
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
}

void ATimePuzzle::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (bIsPuzzleTriggered)
		return;

	bIsPuzzleTriggered = true;

	for (int i = 0; i < 3; ++i)
	{
		Actors[i]->SetActorHiddenInGame(false);
	}

	Car = Cast<AToyCar>(OtherActor);

	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->BeginTimer();
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

void ATimePuzzle::PuzzleComplete()
{
	bIsOpeningDoor = true;

	if (CameraDirector != nullptr)
		CameraDirector->BeginTimePuzzleCameraChange(Car);

	if (Car != nullptr)
		Car->SetCanMove(false);

	iDoorTime = MAX_DOOR_TIMER;
	GetWorldTimerManager().SetTimer(DoorTimer, this, &ATimePuzzle::OpenDoor, 1.0f, true, 0.0f);
}

void ATimePuzzle::PointManage()
{
	if (!Actors.Num())
		return;

	TArray<UPrimitiveComponent*> components;

	Actors[0]->GetOverlappingComponents(components);

	for (UPrimitiveComponent* pComponent : components)
	{
		if (pComponent->GetCollisionObjectType() == ECollisionChannel::ECC_Vehicle)
		{
			Actors[0]->SetActorHiddenInGame(true);
			Actors.RemoveAt(0);
			for (int i = 0; i < GetVisibleActors(); ++i)
			{
				Actors[i]->SetActorHiddenInGame(false);
			}
			return;
		}
	}
}

int ATimePuzzle::GetVisibleActors()
{
	if (Actors.Num() >= 3)
		return 3;
	return Actors.Num();
}

bool ATimePuzzle::AllPointsCollected()
{
	if (!Actors.Num())
		return true;

	return false;
}

void ATimePuzzle::PuzzleFailed()
{
	bIsPuzzleTriggered = false;
	iDoorTime = MAX_DOOR_TIMER;
	for (AActor* Actor : Actors)
	{
		Actor->SetActorHiddenInGame(true);
	}
	Actors = CopyActors;
}