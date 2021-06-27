// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorScript.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UInstancedStaticMeshComponent;

UCLASS()
class HALFLIFEEXPERIMENT_API ADoorScript : public AActor
{

	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ADoorScript();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OpenDoor(AHalfLifeExperimentCharacter* character);

	UPROPERTY()
	UStaticMeshComponent* Door;
	
	UPROPERTY()
	FTimerHandle UnusedHandle;

	UFUNCTION(BlueprintCallable)
	void SetDoorMesh(UStaticMeshComponent* mesh);

	UFUNCTION()
	void RotateTheDoor();
};
