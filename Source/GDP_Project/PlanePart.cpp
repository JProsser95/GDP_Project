// Fill out your copyright notice in the Description page of Project Settings.

#include "PlanePart.h"
#include "PossessableActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "TimePuzzle.h"
#include "FrictionPuzzle.h"
#include "GDP_ProjectGameModeBase.h"
#include "ToyCar.h"
#include "Macros.h"
#include "AchievementManager.h"

#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"


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

	static ConstructorHelpers::FObjectFinder<USoundCue> collectSound(TEXT("/Game/Sounds/Yeah_Cue"));

	CollectSound = collectSound.Object;

	// Create an audio component, the audio component wraps the Cue
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

	// I don't want the sound playing the moment it's created.
	AudioComponent->bAutoActivate = false;

	AudioComponent->SetSound(CollectSound);

	AudioComponent->SetVolumeMultiplier(2.0f);
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

	FQuat Quaternion = this->GetActorRotation().Quaternion();
	// Rotate around the world Z axis:
	Quaternion *= FQuat(FVector::UpVector, FMath::DegreesToRadians(20.0f * DeltaTime));
	this->SetActorRotation(Quaternion);

}

void APlanePart::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (/*bIsCollected || */!OtherActor->FindComponentByClass<UPossessableActorComponent>())
		return;

	AToyCar* pToyCar = CastChecked<AToyCar>(OtherActor);
	pToyCar->UpdatePlanePartsInHUD(_PartName);

	AudioComponent->Play();

	UE_LOG(LogTemp, Warning, TEXT("Sound should be playing"));

	CollectPart();
}

void APlanePart::CollectPart()
{

	switch (_PartName)
	{
	case PROPELLER:

		for (TActorIterator<AAchievementManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ActorItr->EarnAchievement(AchievementName::PLANE_PROP);
		}
		for (TActorIterator<AToyCar> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ActorItr->SetPuzzleCompleted(PuzzleName::TRAIN);
		}
		break;

	case HULL:

		for (TActorIterator<AAchievementManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ActorItr->EarnAchievement(AchievementName::PLANE_HULL);
		}
		break;

	case WINGS:

		for (TActorIterator<AAchievementManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ActorItr->EarnAchievement(AchievementName::PLANE_WING);
		}
		break;

	}

	//AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	//GameMode->SetPlanePartCollected(_PartName);

	//this->Destroy();
	this->SetActorEnableCollision(false);
	this->SetActorHiddenInGame(true);
}
