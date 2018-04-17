// Fill out your copyright notice in the Description page of Project Settings.

#include "GameEnder.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "GDP_ProjectGameModeBase.h"

// Sets default values
AGameEnder::AGameEnder()
	: fTimer(3.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	TriggerBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGameEnder::OnBeginOverlap);
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));
	TriggerBox->SetWorldScale3D(FVector(4.25f, 3.5f, 4.75f));
}

// Called when the game starts or when spawned
void AGameEnder::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGameEnder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (fTimer == 3.0f)
	{
		TArray<UPrimitiveComponent*> components;

		this->GetOverlappingComponents(components);

		for (UPrimitiveComponent* pComponent : components)
		{

			UE_LOG(LogTemp, Warning, TEXT("Game has been completed!"));
			AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
			GameMode->SetHintHUD("Victory Achieved");
			fTimer -= DeltaTime;
		}

	}
	else
	{
		if (fTimer <= 0.0f)
			UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");

		fTimer -= DeltaTime;
	}
}

void AGameEnder::OnBeginOverlap(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Game has been completed!"));
}

