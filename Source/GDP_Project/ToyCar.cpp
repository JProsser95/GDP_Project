// Fill out your copyright notice in the Description page of Project Settings.

#include "ToyCar.h"
#include "ToyPlane.h"
#include "ToyCarWheelFront.h"
#include "ToyCarWheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "Components/SphereComponent.h"
#include "GDP_ProjectGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "PossessableActorComponent.h"
#include "EngineUtils.h"
#include "RespawnPoint.h"
#include "Macros.h"

const FName AToyCar::LookUpBinding("LookUp");
const FName AToyCar::LookRightBinding("LookRight");

AToyCar::AToyCar() 
{
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("SkeletalMesh'/Game/Car/TOYCAR.TOYCAR'"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Car/TOYCAR_Skeleton_AnimBlueprint"));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	SphereCollider->SetRelativeScale3D(FVector(7.0f, 7.0f, 7.0f));
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AToyCar::OnBeginOverlap);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &AToyCar::OnEndOverlap);

	// Setup friction materials
	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> SlipperyMat(TEXT("/Game/VehicleAdv/PhysicsMaterials/Slippery.Slippery"));
	SlipperyMaterial = SlipperyMat.Object;
	
	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> NonSlipperyMat(TEXT("/Game/VehicleAdv/PhysicsMaterials/NonSlippery.NonSlippery"));
	NonSlipperyMaterial = NonSlipperyMat.Object;

	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);

	// Wheels/Tyres
	// Setup the wheels
	Vehicle4W->WheelSetups[0].WheelClass = UToyCarWheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("WheelFL");
	//Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -8.f, 0.f);

	Vehicle4W->WheelSetups[1].WheelClass = UToyCarWheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("WheelFR");
	//Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 8.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = UToyCarWheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("WheelRL");
	//Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -8.f, 0.f);
	
	Vehicle4W->WheelSetups[3].WheelClass = UToyCarWheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("WheelRR");
	//Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 8.f, 0.f);

	// Adjust the tire loading
	Vehicle4W->MinNormalizedTireLoad = 0.0f;
	Vehicle4W->MinNormalizedTireLoadFiltered = 0.2f;
	Vehicle4W->MaxNormalizedTireLoad = 2.0f;
	Vehicle4W->MaxNormalizedTireLoadFiltered = 2.0f;

	// Engine 
	// Torque setup
	Vehicle4W->MaxEngineRPM = 2000.0f;
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 420.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.0f, 400.0f);

	// Adjust the steering 
	Vehicle4W->SteeringCurve.GetRichCurve()->Reset();
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(26.0f, 0.5f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(78.0f, 0.2f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(122.0f, 0.13f);


	// Transmission	
	// We want 4wd
	Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;

	// Drive the front wheels a little more than the rear
	Vehicle4W->DifferentialSetup.FrontRearSplit = 0.65;

	// Automatic gearbox
	Vehicle4W->TransmissionSetup.bUseGearAutoBox = true;
	Vehicle4W->TransmissionSetup.GearSwitchTime = 0.15f;
	Vehicle4W->TransmissionSetup.GearAutoBoxLatency = 1.0f;

	// Physics settings
	// Adjust the center of mass - the buggy is quite low
	UPrimitiveComponent* UpdatedPrimitive = Cast<UPrimitiveComponent>(Vehicle4W->UpdatedComponent);
	if (UpdatedPrimitive)
	{
		UpdatedPrimitive->BodyInstance.COMNudge = FVector(8.0f, 0.0f, 0.0f);
	}

	// Set the inertia scale. This controls how the mass of the vehicle is distributed.
	Vehicle4W->InertiaTensorScale = FVector(1.0f, 1.333f, 1.2f);

	// Create a spring arm component for our chase camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 34.0f));
	SpringArm->SetWorldRotation(FRotator(-20.0f, 0.0f, 0.0f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 125.0f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;

	// Create the chase camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeLocation(FVector(-125.0, 0.0f, 0.0f));
	Camera->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	bIsLowFriction = false;
	bInReverseGear = false;

	bCanPosses = false;

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AToyCar::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AToyCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;

	// Update phsyics material
	UpdatePhysicsMaterial();
}

void AToyCar::Restart()
{
	Super::Restart();

	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->ChangeHUD("ToyCar");
}

// Called to bind functionality to input
void AToyCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("CarMoveForward", this, &AToyCar::MoveForward);
	PlayerInputComponent->BindAxis("CarMoveRight", this, &AToyCar::MoveRight);

	PlayerInputComponent->BindAction("CarHandbrake", IE_Pressed, this, &AToyCar::OnHandbrakePressed);
	PlayerInputComponent->BindAction("CarHandbrake", IE_Released, this, &AToyCar::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("Posses", IE_Released, this, &AToyCar::ChangePossesion);
	PlayerInputComponent->BindAction("ResetCar", IE_Released, this, &AToyCar::ResetPositionAndRotation);
	PlayerInputComponent->BindAction("RespawnCar", IE_Released, this, &AToyCar::Respawn);

}

void AToyCar::MoveForward(float AxisValue)
{
	GetVehicleMovementComponent()->SetThrottleInput(AxisValue);
}

void AToyCar::MoveRight(float AxisValue)
{
	GetVehicleMovementComponent()->SetSteeringInput(AxisValue);
}

void AToyCar::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AToyCar::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AToyCar::UpdatePhysicsMaterial()
{
	if (GetActorUpVector().Z < 0)
	{
		if (bIsLowFriction == true)
		{
			GetMesh()->SetPhysMaterialOverride(NonSlipperyMaterial);
			bIsLowFriction = false;
		}
		else
		{
			GetMesh()->SetPhysMaterialOverride(SlipperyMaterial);
			bIsLowFriction = true;
		}
	}
}

void AToyCar::ChangePossesion()
{
	//AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	//GameMode->RemoveHUD();
	if (possesActor != nullptr)
		GetWorld()->GetFirstPlayerController()->Possess(possesActor);
}

void AToyCar::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (bCanPosses || !OtherActor->FindComponentByClass<UPossessableActorComponent>())
		return;

	OUTPUT_STRING("CAR HIT");

	bCanPosses = true;
	possesActor = Cast<APawn>(OtherActor);
	//AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	//GameMode->AddHUD();
}

void AToyCar::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bCanPosses)
		return;

	OUTPUT_STRING("CAR FINISH HIT");


	bCanPosses = false;
	possesActor = nullptr;
	//AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	//GameMode->RemoveHUD();
}

void AToyCar::ResetPositionAndRotation()
{
	OUTPUT_STRING("Reset");

	FVector currentLoaction = this->GetActorLocation();
	FRotator currentRotation = this->GetActorRotation();

	this->SetActorRelativeLocation(FVector(currentLoaction.X, currentLoaction.Y, currentLoaction.Z + 100), false, NULL, ETeleportType::TeleportPhysics);
	this->SetActorRotation(FRotator(0,0,0), ETeleportType::TeleportPhysics);
}

void AToyCar::Respawn()
{
	for (TActorIterator<ARespawnPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetIsCurrentSpawnPoint())
		{
			FVector spawnLocation = ActorItr->GetActorLocation();
			spawnLocation.Z += 100;
			this->SetActorLocationAndRotation(spawnLocation, FRotator(0, 0, 0), false, NULL, ETeleportType::TeleportPhysics);
		}
	}
}