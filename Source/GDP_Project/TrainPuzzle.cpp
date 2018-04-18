// Fill out your copyright notice in the Description page of Project Settings.

#include "TrainPuzzle.h"
#include "EngineUtils.h"
#include "AchievementManager.h"


// Sets default values
ATrainPuzzle::ATrainPuzzle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PuzzleResetBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PuzzleResetBoxTriggerBox"));
	PuzzleResetBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	PuzzleResetBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));
	PuzzleResetBox->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

}

// Called when the game starts or when spawned
void ATrainPuzzle::BeginPlay()
{
	Super::BeginPlay();

	TrainPuzzleStates.SetNum(Triggers.Num()); // This makes sure that there are the same number of states and triggers

	CameraNumber = 0;
	ShowingFailureCamera = false;

	// Get the ToyCar that is now in the scene
	for (TActorIterator<AToyCar> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if ((*ActorItr)->AutoPossessPlayer == EAutoReceiveInput::Player0)
			ToyCar = *ActorItr;
	}
}

// Called every frame
void ATrainPuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShowingFailureCamera)
		return;

	CheckAndUpdateTriggers();

	if (ToyCar && PuzzleResetBox->IsOverlappingActor(ToyCar))
	{
		for (TActorIterator<AAchievementManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ActorItr->EarnAchievement(AchievementName::OFF_THE_RAILS);
		}
		ResetToLastCheckpoint();
	}

	if (ToyTrain->TrainPuzzleFailed())
	{
		for (TActorIterator<AAchievementManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ActorItr->EarnAchievement(AchievementName::OFF_THE_RAILS);
		}
		ShowingFailureCamera = true;
		RunFailureCamera(CameraNumber);
	}
}

void ATrainPuzzle::CheckAndUpdateTriggers()
{
	for (int i = 0; i < Triggers.Num(); ++i)
	{
		if (Triggers[i] && !ToyTrain->OnFailureTrainLine() && Triggers[i]->IsOverlappingActor(ToyCar))
		{
			Triggers[i]->Destroy();
			Triggers[i] = nullptr;

			TrainPuzzleStates[i].CarPosition = ToyCar->GetActorLocation();
			TrainPuzzleStates[i].CarRotation = ToyCar->GetActorRotation();

			CameraNumber = i;

			if (i == 1)
				TrainPuzzleStates[i].TrainSplineCounter = 1000;
			else
				TrainPuzzleStates[i].TrainSplineCounter = ToyTrain->GetSplineCounter();

			TrainPuzzleStates[i].TrainState = ToyTrain->GetTrainState();
		}
	}
}

void ATrainPuzzle::ResetToLastCheckpoint()
{
	for (int i = Triggers.Num() - 1; i >= 0; --i) // Count backwards so we get the latest checkpoint
	{
		if (!Triggers[i]) // If the trigger has already been destroyed then that checkpoint has been saved
		{
			ToyCar->SetActorRelativeLocation(TrainPuzzleStates[i].CarPosition, false, NULL, ETeleportType::TeleportPhysics);
			ToyCar->SetActorRotation(FQuat(TrainPuzzleStates[i].CarRotation), ETeleportType::TeleportPhysics);
			ToyTrain->SetSplineCounter(TrainPuzzleStates[i].TrainSplineCounter);
			ToyTrain->SetTrainState(TrainPuzzleStates[i].TrainState);
			ShowingFailureCamera = false;
			if (i == 0) // First trigger
				ToyTrain->SetMovementDirection(-1);
			return; // We're done, return out
		}
	}
}
