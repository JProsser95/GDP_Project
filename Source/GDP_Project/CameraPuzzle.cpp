// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraPuzzle.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SceneComponent.h"
#include "CameraDirector.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Camera.h"
#include "Macros.h"
#include "ToyCar.h"

//const int MAX_SAFE_HEIGHT(450);
const int MAX_SAFE_TIMER(6);

// Sets default values
ACameraPuzzle::ACameraPuzzle()
	: bIsActive(false), bPuzzleFailed(false), bIsClosingSafe(false), bIsOpeningSafe(false)
	 ,iBrighter(1), iSafeTime(MAX_SAFE_TIMER + 1), fLightIntensity(0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Safe = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	Safe->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetBody(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (MeshAssetBody.Object)
		Safe->SetStaticMesh(MeshAssetBody.Object);

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

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACameraPuzzle::OnBeginOverlap);
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	DirectionalLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("LightComponent"));
	DirectionalLight->SetupAttachment(RootComponent);
	DirectionalLight->SetWorldRotation(FRotator(-90, 0, 0));
	DirectionalLight->SetIntensity(0);
	DirectionalLight->SetLightColor(FColor(255, 0, 40));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACameraPuzzle::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<ACamera> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		Cameras.Add(*ActorItr);
	}

	// Get the Camera Director that is in the scene
	for (TActorIterator<ACameraDirector> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		CameraDirector = *ActorItr;
	}

	//fOpenSafeHeight = MeshComponent->GetComponentLocation().Z - MAX_SAFE_HEIGHT;
	//fClosedSafeHeight = MeshComponent->GetComponentLocation().Z;

}

// Called every frame
void ACameraPuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//For the widget testing
	FVector PlayerLoc = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), PlayerLoc);
	TriggerWidget->SetRelativeRotation(PlayerRot);
	CompleteWidget->SetRelativeRotation(PlayerRot);

	//if (bIsActive)
	//{
	//	if (MeshComponent->GetComponentLocation().Z >= fOpenSafeHeight)
	//	{
	//		FVector SafeLocation = MeshComponent->GetComponentLocation();
	//		MeshComponent->SetWorldLocation(SafeLocation - FVector(0, 0, DeltaTime * 100));
	//	}
	//}

	if (bIsClosingSafe)
	{
		FVector SafeLocation = Safe->GetComponentLocation();
		Safe->SetWorldLocation(SafeLocation + FVector(0, 0, DeltaTime * 100));
	}

	if (bIsOpeningSafe)
	{
		FVector SafeLocation = Safe->GetComponentLocation();
		Safe->SetWorldLocation(SafeLocation - FVector(0, 0, DeltaTime * 100));
	}


	if (bPuzzleFailed)
	{
		//if (MeshComponent->GetComponentLocation().Z <= fClosedSafeHeight)
		//{
		//	FVector SafeLocation = MeshComponent->GetComponentLocation();
		//	MeshComponent->SetWorldLocation(SafeLocation + FVector(0, 0, DeltaTime * 100));
		//}

		if (fLightIntensity >= 10.0f)
			iBrighter = -1;
		else if (fLightIntensity <= 0.0f)
			iBrighter = 1;

		fLightIntensity += 10.0f * DeltaTime * iBrighter;
		DirectionalLight->SetIntensity(fLightIntensity);
	}
}

void ACameraPuzzle::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (bIsActive)
		return;

	bIsActive = true;
	bPuzzleFailed = false;
	bIsOpeningSafe = true;

	DirectionalLight->SetIntensity(0);

	if (CameraDirector != nullptr)
		CameraDirector->BeginCameraPuzzleCameraChange();

	Car = Cast<AToyCar>(OtherActor);

	if (Car != nullptr)
		Car->SetCanMove(false);

	for (ACamera* Actor : Cameras)
	{
		Actor->SetIsActive(true);
	}

	iSafeTime = MAX_SAFE_TIMER;
	GetWorldTimerManager().SetTimer(SafeTimer, this, &ACameraPuzzle::OpenSafe, 1.0f, true, 0.0f);

}

void ACameraPuzzle::PuzzleFailed()
{
	bIsActive = false;
	bPuzzleFailed = true;
	iSafeTime = MAX_SAFE_TIMER;
	GetWorldTimerManager().SetTimer(SafeTimer, this, &ACameraPuzzle::CloseSafe, 1.0f, true, 0.0f);
}

void ACameraPuzzle::CloseSafe()
{
	if (--iSafeTime <= 0)
	{
		bIsClosingSafe = false;
		iSafeTime = MAX_SAFE_TIMER;
		GetWorldTimerManager().ClearTimer(SafeTimer);
	}
}

void ACameraPuzzle::OpenSafe()
{
	if (--iSafeTime <= 0)
	{
		bIsOpeningSafe = false;
		iSafeTime = MAX_SAFE_TIMER;
		if (CameraDirector != nullptr)
			CameraDirector->BeginCameraPuzzleCameraChange();
		if (Car != nullptr)
			Car->SetCanMove(true);
		GetWorldTimerManager().ClearTimer(SafeTimer);
	}
}