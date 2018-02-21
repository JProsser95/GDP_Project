// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera.h"
#include "CameraPuzzle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "PossessableActorComponent.h"
#include "EngineUtils.h"
#include "Macros.h"

const int WAIT_TIME(3);
const int ACTION_TIME(7);

// Sets default values
ACamera::ACamera()
	: eDirection(CLOCKWISE), rOriginalRotation(0, 0, 0), bIsActive(false), bIsRotating(false), iWaitTime(WAIT_TIME), iActionTime(ACTION_TIME)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));


	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetBody(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (MeshAssetBody.Object)
		MeshComponent->SetStaticMesh(MeshAssetBody.Object);

	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComponent"));
	SpotLightComponent->SetupAttachment(MeshComponent);
	SpotLightComponent->Intensity = 100000;
	SpotLightComponent->InnerConeAngle = 25.0f;
	SpotLightComponent->OuterConeAngle = 31.0f;
	SpotLightComponent->LightColor = FColor(255, 153, 181);
	SpotLightComponent->AttenuationRadius = 1500;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	TriggerBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACamera::OnBeginOverlap);
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));
	TriggerBox->SetWorldScale3D(FVector(4.25f, 3.5f, 4.75f));
}

// Called when the game starts or when spawned
void ACamera::BeginPlay()
{
	Super::BeginPlay();

	rOriginalRotation = this->GetActorRotation();
}

// Called every frame
void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActive)
	{
		return;
	}

	if (bIsRotating) {

		FRotator NewRotation = GetActorRotation();

		if (eDirection == CLOCKWISE)
		{
			NewRotation += FRotator(0, 10 * DeltaTime, 0);
		}
		else
		{
			NewRotation -= FRotator(0, 10 * DeltaTime, 0);
		}

		this->SetActorRotation(NewRotation);

	}

	
}

void ACamera::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!bIsActive || !OtherActor->FindComponentByClass<UPossessableActorComponent>())
		return;

	bIsActive = false;

	for (TActorIterator<ACamera> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->SetIsActive(false);
	}

	for (TActorIterator<ACameraPuzzle> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->PuzzleFailed();
	}
}

void ACamera::Wait()
{
	if (--iWaitTime <= 0)
	{
		switch (_CameraType)
		{
		case ROTATING_CAMERA:
			
			if (eDirection == COUNTER_CLOCKWISE)
				eDirection = CLOCKWISE;
			else
				eDirection = COUNTER_CLOCKWISE;
			
			bIsRotating = true;
			break;

		case FLICKERING_CAMERA:

			SpotLightComponent->SetIntensity(100000.0f);
			TriggerBox->SetActive(true);
		}

		iWaitTime = WAIT_TIME;
		GetWorldTimerManager().ClearTimer(WaitTimer);
		GetWorldTimerManager().SetTimer(ActionTimer, this, &ACamera::Action, 1.0f, true, 0.0f);

		
	}
}

void ACamera::Action()
{
	if (--iActionTime <= 0)
	{
		bIsRotating = false;

		iActionTime = ACTION_TIME;
		GetWorldTimerManager().ClearTimer(ActionTimer);
		GetWorldTimerManager().SetTimer(WaitTimer, this, &ACamera::Wait, 1.0f, true, 0.0f);

		if (_CameraType == FLICKERING_CAMERA)
		{
			SpotLightComponent->SetIntensity(0.0f);
			TriggerBox->SetActive(false);
		}
		
	}
}

void ACamera::SetIsActive(bool Value)
{
	bIsActive = Value;

	if (_CameraType == STATIC_CAMERA)
		return;

	if (bIsActive)
		GetWorldTimerManager().SetTimer(ActionTimer, this, &ACamera::Action, 1.0f, true, 0.0f);
	else
	{
		GetWorldTimerManager().ClearTimer(WaitTimer);
		GetWorldTimerManager().ClearTimer(ActionTimer);
		iWaitTime = WAIT_TIME;
		iActionTime = ACTION_TIME;

		if (_CameraType == ROTATING_CAMERA)
		{
			eDirection = CLOCKWISE;
			bIsRotating = true;
		}

		this->SetActorRotation(rOriginalRotation);
	}
}