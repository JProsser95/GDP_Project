// Fill out your copyright notice in the Description page of Project Settings.

#include "PlanePart.h"
#include "PossessableActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "TimePuzzle.h"
#include "FrictionPuzzle.h"
#include "GDP_ProjectGameModeBase.h"

// Sets default values
APlanePart::APlanePart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	StaticMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	//Trigger Box for activating this spawn point
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APlanePart::OnBeginOverlap);
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

}

// Called when the game starts or when spawned
void APlanePart::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlanePart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlanePart::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (/*bIsCollected || */!OtherActor->FindComponentByClass<UPossessableActorComponent>())
		return;

	CollectPart();
}

void APlanePart::CollectPart()
{

	switch (_PartName)
	{
	case PROPELLER:

		// Get the Camera Director that is in the scene
		//for (TActorIterator<ATimePuzzle> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		//{
		//	ActorItr->PuzzleComplete();
		//}
		break;

	case HULL:

		//// Get the Camera Director that is in the scene
		//for (TActorIterator<AFrictionPuzzle> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		//{
		//	ActorItr->PuzzleComplete();
		//}
		break;


	}

	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->SetPlanePartCollected(_PartName);

	this->Destroy();
}