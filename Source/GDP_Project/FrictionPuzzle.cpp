// Fill out your copyright notice in the Description page of Project Settings.

#include "FrictionPuzzle.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "PossessableActorComponent.h"
#include "ToyCar.h"
#include "CleanerObject.h"
#include "Macros.h"

// Sets default values
AFrictionPuzzle::AFrictionPuzzle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	//Trigger Box for activating this spawn point
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFrictionPuzzle::OnBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFrictionPuzzle::OnEndOverlap);
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));

	StickyFloor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StickyFloorComponent"));
	StickyFloor->SetupAttachment(RootComponent);

	bIsCarStuck = false;
}

// Called when the game starts or when spawned
void AFrictionPuzzle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFrictionPuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFrictionPuzzle::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->FindComponentByClass<UCleanerObject>())
	{
		OUTPUT_STRING("CLEAN");
		OtherActor->Destroy();
		this->Destroy();
		return;
	}

	if (bIsCarStuck || !OtherActor->FindComponentByClass<UPossessableActorComponent>())
	{
		return;
	}
	OUTPUT_STRING("Stick");
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
	OUTPUT_STRING("UnSTICK");

	AToyCar* Car = Cast<AToyCar>(OtherActor);
	Car->OffSticky();
	bIsCarStuck = false;
}