#include "PawnTest.h"
#include "CustomMovementComponent.h"

// Sets default values
APawnTest::APawnTest()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Our root component will be a sphere that reacts to physics
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));

	//UStaticMeshComponent* SphereComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Planemain"));
	RootComponent = SphereComponent;
	SphereComponent->InitSphereRadius(100.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	// Create and position a mesh component so we can see where our sphere is
	/*UStaticMeshComponent* SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	SphereVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/Content/Plane/Planemain"));
	if (SphereVisualAsset.Succeeded())
	{
	SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
	SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
	SphereVisual->SetWorldScale3D(FVector(0.8f));
	}*/

	//// Create a particle system that we can activate or deactivate
	//OurParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MovementParticles"));
	//OurParticleSystem->SetupAttachment(SphereVisual);
	//OurParticleSystem->bAutoActivate = false;
	//OurParticleSystem->SetRelativeLocation(FVector(-20.0f, 0.0f, 20.0f));
	//static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire"));
	//if (ParticleAsset.Succeeded())
	//{
	//	OurParticleSystem->SetTemplate(ParticleAsset.Object);
	//}

	// Use a spring arm to give the camera smooth, natural-feeling motion.
	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->RelativeRotation = FRotator(-45.f, 0.f, 0.f);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;

	// Create a camera and attach to our spring arm
	UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ActualCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	// Take control of the default player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create an instance of our movement component, and tell it to update our root component.
	CustomMovementComponent = CreateDefaultSubobject<UCustomMovementComponent>(TEXT("CustomMovementComponent"));
	CustomMovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void APawnTest::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APawnTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APawnTest::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	//InputComponent->BindAction("ParticleToggle", IE_Pressed, this, &APawnTest::ParticleToggle);

	InputComponent->BindAxis("MoveForward", this, &APawnTest::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APawnTest::MoveRight);
	InputComponent->BindAxis("Turn", this, &APawnTest::Turn);
}

UPawnMovementComponent* APawnTest::GetMovementComponent() const
{
	return CustomMovementComponent;
}

void APawnTest::MoveForward(float AxisValue)
{
	if (CustomMovementComponent && (CustomMovementComponent->UpdatedComponent == RootComponent))
	{
		CustomMovementComponent->AddInputVector(GetActorForwardVector() * AxisValue * 10);
	}
}

void APawnTest::MoveRight(float AxisValue)
{
	if (CustomMovementComponent && (CustomMovementComponent->UpdatedComponent == RootComponent))
	{
		CustomMovementComponent->AddInputVector(GetActorRightVector() * AxisValue * 10);
	}
}

void APawnTest::Turn(float AxisValue)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += AxisValue;
	SetActorRotation(NewRotation);
}

//void APawnTest::ParticleToggle()
//{
//	if (OurParticleSystem && OurParticleSystem->Template)
//	{
//		OurParticleSystem->ToggleActive();
//	}
//}