// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GravityGun.h"
#include "HalfLifeExperimentCharacter.generated.h"


class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class AHalfLifeExperimentCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** Location Location where to hold the object. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_ObjectCapture;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UPhysicsHandleComponent* PhysicsHandle;

public:
	AHalfLifeExperimentCharacter();

	//Dash system
	UPROPERTY(EditAnywhere, Category = Dash)
		float DashDistance;
	UPROPERTY(EditAnywhere, Category = Dash)
		float DashSpeed;
	UPROPERTY(EditAnywhere, Category = Dash)
		float DashCooldown;
	UPROPERTY()
		bool canDash;
	UPROPERTY()
		FTimerHandle UnusedHandle;
	UFUNCTION()
		void StopDashing();
	UFUNCTION()
		void ResetDash();
	UFUNCTION()
		void MoveDash();

protected:
	virtual void BeginPlay();

public:
	virtual void Tick(float DeltaTime) override;
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AHalfLifeExperimentProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

protected:
	
	/** Fires a projectile. */
	void OnFire();

	void OnHolding(FHitResult hit);

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	//Gravity Gun System
	UPROPERTY(EditAnywhere, Category = GravityGun)
	float FireRange;

	UPROPERTY(EditAnywhere, Category = GravityGun)
	float FireForce;
	bool hasObject;
	FHitResult TheObject;

	//Point System
	float Point;

	UFUNCTION(BlueprintCallable)
		void SetPoint(int getPoint);

	UFUNCTION(BlueprintCallable)
		int getPoint();

	// Door and Key System

	UFUNCTION()
	void OnInteract();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	TArray<FString> itemList;

	UPROPERTY(EditAnywhere, Category = Item)
	float InteractRange;

	UFUNCTION(BlueprintCallable)
	void addItemList(FString item);

	UFUNCTION(BlueprintCallable)
	FString GetItemList(int i);

	UFUNCTION(BlueprintCallable)
	bool CheckItemFromList(FString item);

	//Jet Pack System

	UFUNCTION()
	void JumpJetPack();

	UFUNCTION()
	void OnJump();

	UFUNCTION()
	void StopJump();

	UPROPERTY(EditAnywhere, Category = JetPack)
	float Trust;
	
	UPROPERTY(EditAnywhere, Category = JetPack)
	float JumpForce;
	
	UPROPERTY(EditAnywhere, Category = JetPack)
	float DelayTimer;

	bool isTrust;

	bool isOnAir;

};

