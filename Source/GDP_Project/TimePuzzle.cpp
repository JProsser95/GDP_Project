// Fill out your copyright notice in the Description page of Project Settings.

#include "TimePuzzle.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SceneComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GDP_ProjectGameModeBase.h"
#include "EngineUtils.h"
#include "Macros.h"

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

	bIsPuzzleTriggered = false;
}

// Called when the game starts or when spawned
void ATimePuzzle::BeginPlay()
{
	Super::BeginPlay();
	
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
				bIsPuzzleTriggered = false;
				GameMode->RemoveTimerWidget();
				FVector DoorLocation = Door->GetComponentLocation();
				Door->SetWorldLocation(DoorLocation - FVector(0, 0, 400.0f));
			}
		}
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

	OUTPUT_STRING("Puzzle Triggered");

	bIsPuzzleTriggered = true;
	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->BeginTimer();

	FVector DoorLocation = Door->GetComponentLocation();
	Door->SetWorldLocation(DoorLocation + FVector(0, 0, 400.0f));
	//Door->SetRelativeLocation(FVector(0, 0, 0.0f));
}
