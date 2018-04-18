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
#include "BackgroundMusicManager.h"
#include "Materials/Material.h"
#include "PlanePart.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"


// Sets default values
ATimePuzzle::ATimePuzzle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	//Trigger Box for activating this spawn point
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	TriggerBox->SetupAttachment(RootComponent);
	//TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATimePuzzle::OnBeginOverlap);
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	Chair = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	Chair->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	Chair->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetBody(TEXT("StaticMesh'/Game/Assets/Chair/KitchenChair.KitchenChair'"));
	if (MeshAssetBody.Object)
		Chair->SetStaticMesh(MeshAssetBody.Object);

	bIsPuzzleTriggered = false;
	bIsPuzzleComplete = false;
	ResizeScale = 1.0f;
	ResizeRate = 2.0f;
	RingSmallScale = 0.1f;

	static ConstructorHelpers::FObjectFinder<USoundCue> powerUpSound(TEXT("/Game/Sounds/Power_Up_Cue"));

	PowerUpSound = powerUpSound.Object;

	// Create an audio component, the audio component wraps the Cue
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

	// I don't want the sound playing the moment it's created.
	AudioComponent->bAutoActivate = false;

	AudioComponent->SetSound(PowerUpSound);

	AudioComponent->SetVolumeMultiplier(2.0f);
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
		Actors[i]->SetActorRelativeScale3D(FVector(RingSmallScale));
	}

	CopyActors = Actors;
}

// Called every frame
void ATimePuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPuzzleTriggered) 
	{
		PointManage(DeltaTime);
	}
}

//void ATimePuzzle::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
//{
//	if (bIsPuzzleTriggered || bIsPuzzleComplete)
//		return;
//	
//}

void ATimePuzzle::ActivatePuzzle(AToyCar* toycar)
{
	//if (!Car)
	//{
	//	Car = Cast<AToyCar>(OtherActor);
	//}

	Car = toycar;

	if (Car->GetIsInPuzzle())
		return;

	// Get the Camera Director that is in the scene
	for (TActorIterator<ABackgroundMusicManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->SetSound(Sounds::TIMER_PUZZLE);
	}

	Car->SetIsInPuzzle(true);
	bIsPuzzleTriggered = true;

	for (int i = 0; i < 3; ++i)
	{
		Actors[i]->SetActorHiddenInGame(false);
		Actors[i]->SetActorRelativeScale3D(FVector(RingSmallScale));
	}

	//BeginPuzzleTimer(60);
	BeginPuzzle();
}

void ATimePuzzle::PuzzleComplete()
{
	bIsPuzzleTriggered = false;
	bIsPuzzleComplete = true;

	if (CameraDirector != nullptr)
		CameraDirector->BeginTimePuzzleCameraChange(Car);

	Chair->SetWorldRotation(FRotator(25.0f, -180.0f, 0.0f));
	FVector Position = Chair->GetComponentLocation();
	Chair->SetWorldLocation(Position + FVector(0.0f, 0.0f, 50.0f));

	// Get the Camera Director that is in the scene
	for (TActorIterator<ABackgroundMusicManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->SetSound(Sounds::MAIN);
	}

	Car->SetIsInPuzzle(false);
	Car->SetPuzzleCompleted(PuzzleName::TIMER);
	Car->CanSeeHints = true;

	//Chair->SetSimulatePhysics(true);

	RemovePuzzleTimer();
	BeginChairTimer();
}

void ATimePuzzle::PointManage(float DeltaTime)
{
	if (!Actors.Num())
		return;

	if (Actors[0]->IsOverlappingActor(Car))
	{
		Actors[0]->SetActorHiddenInGame(true);
		Actors.RemoveAt(0);
		AudioComponent->Play();
		if (Actors.Num())
			Actors[GetVisibleActors() - 1]->SetActorHiddenInGame(false);
	}

	if (AllPointsCollected())
	{
		PuzzleComplete();
	}
	else 
	{
		float scale = Actors[0]->GetActorScale3D().X;
		float newScale(scale + (DeltaTime * ResizeRate));

		if (newScale <= ResizeScale)
			scale = newScale;
		else
			scale = ResizeScale;

		Actors[0]->SetActorRelativeScale3D(FVector(scale));
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
	for (AActor* Actor : Actors)
	{
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorRelativeScale3D(FVector(RingSmallScale));
	}

	Car->SetIsInPuzzle(false);
	
	Actors = CopyActors;
}

