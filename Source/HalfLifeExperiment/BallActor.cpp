// Fill out your copyright notice in the Description page of Project Settings.


#include "BallActor.h"
#include "Math/UnrealMathUtility.h"
#include "Materials/Material.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ABallActor::ABallActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABallActor::BeginPlay()
{
	Super::BeginPlay();
	Point = 1;
}

// Called every frame
void ABallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABallActor::SetPointValue(TArray<UMaterialInstance*> material, UStaticMeshComponent* mesh)
{
	Point = FMath::RandRange(1, 5);
	Point = FMath::RoundFromZero(Point);
	mesh->SetMaterial(0, material[Point-1]);
}

float ABallActor::GetPointValue()
{
	return Point;
}

FString ABallActor::PrintPoint()
{
	FString string = "Hello World";
	return string;
}





