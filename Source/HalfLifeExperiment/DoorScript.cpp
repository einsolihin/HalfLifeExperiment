// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorScript.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "HalfLifeExperimentCharacter.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TimelineComponent.h"


// Sets default values
ADoorScript::ADoorScript()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADoorScript::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoorScript::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoorScript::SetDoorMesh(UStaticMeshComponent* mesh)
{
	Door = mesh;
}

void ADoorScript::OpenDoor(AHalfLifeExperimentCharacter* character)
{
	if (character->CheckItemFromList("Key"))
	{
		RotateTheDoor();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("It's Lock!"));
		return;
	}
}



void ADoorScript::RotateTheDoor()
{
	FQuat quatRotation = FQuat(FRotator(0, 10, 0));
	Door->AddRelativeRotation(quatRotation, false, 0, ETeleportType::None);
	if (Door->GetRelativeRotation().Yaw < 90)
	{
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &ADoorScript::RotateTheDoor, 0.01, false);
	}
}


