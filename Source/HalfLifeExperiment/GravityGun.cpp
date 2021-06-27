// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityGun.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"



// Sets default values
AGravityGun::AGravityGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create a gun mesh component

	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	//FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 57.f, 10.6f));

	FP_ObjectCapture = CreateDefaultSubobject<USceneComponent>(TEXT("ObjectCapture"));
	FP_ObjectCapture->SetupAttachment(FP_Gun);
	FP_ObjectCapture->SetRelativeLocation(FVector(0.2f, 100.f, 10.6f));

	bHolding = false;
}

// Called when the game starts or when spawned
void AGravityGun::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGravityGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGravityGun::OnFire()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Fire !!"));
}

void AGravityGun::OnPickUp(float range)
{
}


