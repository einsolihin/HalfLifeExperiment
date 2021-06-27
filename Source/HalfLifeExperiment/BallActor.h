// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallActor.generated.h"

UCLASS()
class HALFLIFEEXPERIMENT_API ABallActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABallActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float Point;

	UFUNCTION(BlueprintCallable)
		void SetPointValue(TArray<UMaterialInstance*> material, UStaticMeshComponent* mesh);

	UFUNCTION(BlueprintCallable)
		float GetPointValue();
	
	UFUNCTION(BlueprintCallable)
		FString PrintPoint();



};
