// Fill out your copyright notice in the Description page of Project Settings.

#include "ReturnPortal.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "PossessableActorComponent.h"
#include "Engine/LevelScriptActor.h"
#include "OutputDeviceNull.h"



// Sets default values
AReturnPortal::AReturnPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	WarpRoomReturnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ReturnPoint"));
	WarpRoomReturnPoint->SetupAttachment(RootComponent);


	//Trigger Box for activating this spawn point
	CameraPuzzleTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CameraPuzzleTriggerBox"));
	CameraPuzzleTriggerBox->SetupAttachment(RootComponent);
	CameraPuzzleTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AReturnPortal::OnBeginOverlap);
	CameraPuzzleTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	//Trigger Box for activating this spawn point
	TimerPuzzleTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TimerPuzzleTriggerBox"));
	TimerPuzzleTriggerBox->SetupAttachment(RootComponent);
	TimerPuzzleTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AReturnPortal::OnBeginOverlap);
	TimerPuzzleTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	//Trigger Box for activating this spawn point
	FrictionPuzzleTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FrictionPuzzleTriggerBox"));
	FrictionPuzzleTriggerBox->SetupAttachment(RootComponent);
	FrictionPuzzleTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AReturnPortal::OnBeginOverlap);
	FrictionPuzzleTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	//Trigger Box for activating this spawn point
	LavaPuzzleTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LavaPuzzleTriggerBox"));
	LavaPuzzleTriggerBox->SetupAttachment(RootComponent);
	LavaPuzzleTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AReturnPortal::OnBeginOverlap);
	LavaPuzzleTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	//For testing
	CameraPuzzleCompleteWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CameraPuzzleCompleteWidget"));
	CameraPuzzleCompleteWidget->SetupAttachment(CameraPuzzleTriggerBox);
	static ConstructorHelpers::FClassFinder<UUserWidget> CameraWidget(TEXT("/Game/TestingPurpose/PuzzleComplete"));
	CameraPuzzleCompleteWidget->SetWidgetClass(CameraWidget.Class);
	CameraPuzzleCompleteWidget->SetRelativeLocation(FVector(0, 0, 150.0f));
	CameraPuzzleCompleteWidget->SetTwoSided(true);

	//For testing
	TimerPuzzleCompleteWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TimerPuzzleCompleteWidget"));
	TimerPuzzleCompleteWidget->SetupAttachment(TimerPuzzleTriggerBox);
	static ConstructorHelpers::FClassFinder<UUserWidget> TimerWidget(TEXT("/Game/TestingPurpose/PuzzleComplete"));
	TimerPuzzleCompleteWidget->SetWidgetClass(TimerWidget.Class);
	TimerPuzzleCompleteWidget->SetRelativeLocation(FVector(0, 0, 150.0f));
	TimerPuzzleCompleteWidget->SetTwoSided(true);

	//For testing
	FrictionPuzzleCompleteWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("FrictionPuzzleCompleteWidget"));
	FrictionPuzzleCompleteWidget->SetupAttachment(FrictionPuzzleTriggerBox);
	static ConstructorHelpers::FClassFinder<UUserWidget> FrictionWidget(TEXT("/Game/TestingPurpose/PuzzleComplete"));
	FrictionPuzzleCompleteWidget->SetWidgetClass(FrictionWidget.Class);
	FrictionPuzzleCompleteWidget->SetRelativeLocation(FVector(0, 0, 150.0f));
	FrictionPuzzleCompleteWidget->SetTwoSided(true);

	//For testing
	LavaPuzzleCompleteWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LavaPuzzleCompleteWidget"));
	LavaPuzzleCompleteWidget->SetupAttachment(LavaPuzzleTriggerBox);
	static ConstructorHelpers::FClassFinder<UUserWidget> LavaWidget(TEXT("/Game/TestingPurpose/PuzzleComplete"));
	LavaPuzzleCompleteWidget->SetWidgetClass(LavaWidget.Class);
	LavaPuzzleCompleteWidget->SetRelativeLocation(FVector(0, 0, 150.0f));
	LavaPuzzleCompleteWidget->SetTwoSided(true);
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

	//For the widget testing
	FVector PlayerLoc = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), PlayerLoc);
	CameraPuzzleCompleteWidget->SetRelativeRotation(PlayerRot);
	TimerPuzzleCompleteWidget->SetRelativeRotation(PlayerRot);
	FrictionPuzzleCompleteWidget->SetRelativeRotation(PlayerRot);
	LavaPuzzleCompleteWidget->SetRelativeRotation(PlayerRot);
}

void AReturnPortal::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!OtherActor->FindComponentByClass<UPossessableActorComponent>())
		return;

	FOutputDeviceNull ar;

	GetLevel()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("HideAllLevels"), ar, NULL, true);

	OtherActor->SetActorLocationAndRotation(WarpRoomReturnPoint->GetComponentLocation(), FRotator(0.0f, 0.0f, 0.0f), false, NULL, ETeleportType::TeleportPhysics);
}