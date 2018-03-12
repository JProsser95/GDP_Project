// Fill out your copyright notice in the Description page of Project Settings.

#include "WaterGlass.h"
#include "CleanerObject.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "CleanerObject.h"
#include "CameraDirector.h"
#include "ToyCar.h"


// Sets default values
AWaterGlass::AWaterGlass()
	: bCouchHit(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	GlassMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlassMeshComponent"));
	GlassMesh->SetupAttachment(RootComponent);

	//Trigger Box for activating this spawn point
	GlassTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GlassTriggerBoxComponent"));
	GlassTriggerBox->SetupAttachment(GlassMesh);
	GlassTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	CouchTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CouchTriggerBoxComponent"));
	CouchTriggerBox->SetupAttachment(RootComponent);
	CouchTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AWaterGlass::OnBeginOverlap);
	CouchTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	Cleaner = CreateDefaultSubobject<UCleanerObject>(TEXT("Cleaner"));

}

// Called when the game starts or when spawned
void AWaterGlass::BeginPlay()
{
	Super::BeginPlay();
	
	// Get the Camera Director that is in the scene
	for (TActorIterator<ACameraDirector> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		CameraDirector = *ActorItr;
	}
}

// Called every frame
void AWaterGlass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWaterGlass::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (bCouchHit)
		return;

	bCouchHit = true;

	if (CameraDirector != nullptr)
		CameraDirector->BeginWaterGlassCameraChange();

	FVector location = GlassMesh->GetComponentLocation();
	GlassMesh->SetWorldLocation(FVector(location.X -20.0f, location.Y, location.Z));

	AToyCar* Car = CastChecked<AToyCar>(OtherActor);
	Car->SetPuzzleCompleted(PuzzleName::FRICTION);
}
