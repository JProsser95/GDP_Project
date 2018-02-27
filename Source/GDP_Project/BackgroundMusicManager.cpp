// Fill out your copyright notice in the Description page of Project Settings.

#include "BackgroundMusicManager.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "UObject/ConstructorHelpers.h"



// Sets default values
ABackgroundMusicManager::ABackgroundMusicManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create an audio component, the audio component wraps the Cue
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

	// I don't want the sound playing the moment it's created.
	AudioComponent->bAutoActivate = false;
	// I want the sound to follow the pawn around, so I attach it to the Pawns root.
	AudioComponent->SetupAttachment(RootComponent);

	// Load our Sound Cue for the propeller sound we created in the editor... 
	static ConstructorHelpers::FObjectFinder<USoundCue> BackgroundSound(TEXT("/Game/Sounds/Background_Audio_Cue"));
	static ConstructorHelpers::FObjectFinder<USoundCue> CameraPuzzleSound(TEXT("/Game/Sounds/CameraPuzzle_Cue"));
	static ConstructorHelpers::FObjectFinder<USoundCue> TimerPuzzleSound(TEXT("/Game/Sounds/TimePuzzle_Cue"));


	AudioCues.Add(BackgroundSound.Object);
	AudioCues.Add(CameraPuzzleSound.Object);
	AudioCues.Add(TimerPuzzleSound.Object);

} 

// Called when the game starts or when spawned
void ABackgroundMusicManager::BeginPlay()
{
	Super::BeginPlay();

	AudioComponent->SetSound(AudioCues[MAIN]);
	AudioComponent->Play();
	currentSoundCue = MAIN;

}

// Called every frame
void ABackgroundMusicManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABackgroundMusicManager::SetSound(Sounds sound)
{
	AudioComponent->FadeOut(1.0f, 0.6f);
	AudioComponent->SetSound(AudioCues[sound]);
	AudioComponent->FadeIn(1.0f);
	currentSoundCue = sound;
}