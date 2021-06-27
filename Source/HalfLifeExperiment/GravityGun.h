// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GravityGun.generated.h"

class USkeletalMeshComponent;
class UPhysicsHandleComponent;
class USphereComponent;

UCLASS(config = Game)
class HALFLIFEEXPERIMENT_API AGravityGun : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_ObjectCapture;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UPhysicsHandleComponent* PhysicsHandle;

	

public:	
	// Sets default values for this actor's properties
	AGravityGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		float PickUpRange;
	UPROPERTY(EditAnywhere)
		float FiringForce;

	bool bHolding;

	UFUNCTION()
		void OnFire();
	UFUNCTION(BlueprintCallable)
		void OnPickUp(float range);

};
