// Fill out your copyright notice in the Description page of Project Settings.

#include "FrictionPuzzle.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "PossessableActorComponent.h"
#include "ToyCar.h"
#include "CleanerObject.h"
#include "Camera/CameraComponent.h"
#include "CameraDirector.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GDP_ProjectGameModeBase.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

const int MAX_CUTAWAY_TIMER(4);


// Sets default values
AFrictionPuzzle::AFrictionPuzzle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	//Trigger Box for activating this spawn point
	StickyTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("StickTriggerBoxComponent"));
	StickyTriggerBox->SetupAttachment(RootComponent);
	StickyTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFrictionPuzzle::OnBeginOverlap);
	StickyTriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFrictionPuzzle::OnEndOverlap);
	StickyTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	CameraEventTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CameraEventTriggerBoxComponent"));
	CameraEventTriggerBox->SetupAttachment(RootComponent);
	CameraEventTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFrictionPuzzle::OnBeginOverlap);
	CameraEventTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	StickyFloor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StickyFloorComponent"));
	StickyFloor->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	bIsCarStuck = false;
	bCameraChanged = false;
	iCameraTime = MAX_CUTAWAY_TIMER;

	static ConstructorHelpers::FObjectFinder<USoundCue> waterSound(TEXT("/Game/Sounds/Water_Cue"));

	SplashSound = waterSound.Object;

	// Create an audio component, the audio component wraps the Cue
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

	// I don't want the sound playing the moment it's created.
	AudioComponent->bAutoActivate = false;


	AudioComponent->SetVolumeMultiplier(4.0f);
}

// Called when the game starts or when spawned
void AFrictionPuzzle::BeginPlay()
{
	Super::BeginPlay();
	AudioComponent->SetSound(SplashSound);
	
	// Get the Camera Director that is in the scene
	for (TActorIterator<ACameraDirector> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		CameraDirector = *ActorItr;
	}
}

// Called every frame
void AFrictionPuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFrictionPuzzle::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->FindComponentByClass<UCleanerObject>() && !bCameraChanged)
	{
		if (CameraDirector != nullptr)
		{
			CameraDirector->BeginFrictionPuzzleCameraChange();
		}
		GetWorldTimerManager().SetTimer(CameraTimer, this, &AFrictionPuzzle::ChangeCamera, 1.0f, true, 0.0f);
		bCameraChanged = true;
		return;
	}

	if (bIsCarStuck || !OtherActor->FindComponentByClass<UPossessableActorComponent>())
	{
		return;
	}

	AToyCar* Car = Cast<AToyCar>(OtherActor);
	Car->OnSticky();
	bIsCarStuck = true;
}

void AFrictionPuzzle::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bIsCarStuck || !OtherActor->FindComponentByClass<UPossessableActorComponent>())
	{
		return;
	}

	AToyCar* Car = Cast<AToyCar>(OtherActor);
	Car->OffSticky();
	bIsCarStuck = false;
}

void AFrictionPuzzle::CleanSticky()
{
	StickyFloor->DestroyComponent();
	AudioComponent->Play();
}

void AFrictionPuzzle::ChangeCamera()
{
	if (--iCameraTime <= 0)
	{
		CameraDirector->BeginFrictionPuzzleCameraChange();
		GetWorldTimerManager().ClearTimer(CameraTimer);
		this->Destroy();
	}

	if (iCameraTime == 2)
	{
		CleanSticky();
	}
}