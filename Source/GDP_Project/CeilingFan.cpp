// Fill out your copyright notice in the Description page of Project Settings.

#include "CeilingFan.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACeilingFan::ACeilingFan()
	:RotationsPerSecond(720.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Mesh
	CeilingCylinderMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CeilingFanCylinderMeshComponent"));
	CeilingFanMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CeilingFanMeshComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FanCylinder(TEXT("StaticMesh'/Game/Assets/Fan/Fan_Cylinder.Fan_Cylinder'"));
	if (FanCylinder.Object)
		CeilingCylinderMeshComponent->SetStaticMesh(FanCylinder.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Fan(TEXT("StaticMesh'/Game/Assets/Fan/Fan.Fan'"));
	if (Fan.Object)
		CeilingFanMeshComponent->SetStaticMesh(Fan.Object);

	CeilingFanMeshComponent->AttachToComponent(CeilingCylinderMeshComponent, FAttachmentTransformRules::KeepWorldTransform);
	RootComponent = CeilingCylinderMeshComponent;

	CeilingFanMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	//PlaneBodyMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AToyPlane::OnToyPlaneOverlap);
	//PlaneBodyMeshComponent->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics, true);
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialPlane(TEXT("MaterialInstanceDynamic'/Game/Plane/Texture/Plane_Material.Plane_Material'"));
	//if (MaterialPlane.Object)
	//	PlaneBodyMeshComponent->SetMaterial(0, MaterialPlane.Object);

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetBody(TEXT("StaticMesh'/Game/Plane/Plane_Plane.Plane_Plane'"));
	//if (MeshAssetBody.Object)
	//	PlaneBodyMeshComponent->SetStaticMesh(MeshAssetBody.Object);
}

// Called when the game starts or when spawned
void ACeilingFan::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACeilingFan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CeilingFanMeshComponent->SetRelativeRotation(FRotator(0.0f, CeilingFanMeshComponent->GetComponentRotation().Yaw + (DeltaTime * RotationsPerSecond), 0.0f));

}

