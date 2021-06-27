// Copyright Epic Games, Inc. All Rights Reserved.

#include "HalfLifeExperimentCharacter.h"
#include "HalfLifeExperimentProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
//Line tracing
#include "DrawDebugHelpers.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "DoorScript.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

#include "Runtime/Engine/Classes/GameFrameWork/CharacterMovementComponent.h"
#include "Runtime/Engine/public/TimerManager.h"


//////////////////////////////////////////////////////////////////////////
// AHalfLifeExperimentCharacter

AHalfLifeExperimentCharacter::AHalfLifeExperimentCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 57.f, 10.f));
	
	FP_ObjectCapture = CreateDefaultSubobject<USceneComponent>(TEXT("ObjectCapture"));
	FP_ObjectCapture->SetupAttachment(FP_Gun);
	FP_ObjectCapture->SetRelativeLocation(FVector(0.2f, 5000.f, 10.f));

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandler"));
	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;


}


void AHalfLifeExperimentCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		//VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		//VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	canDash = true;
	DashCooldown = 1.f;
	hasObject = false;

	isTrust = false;
}

void AHalfLifeExperimentCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PhysicsHandle->SetTargetLocation(FP_ObjectCapture->GetComponentLocation());

	/*FVector Start = GetCapsuleComponent()->GetComponentLocation();
	FVector End = (GetCapsuleComponent()->GetUpVector() * -5) + GetCapsuleComponent()->GetComponentLocation();

	FHitResult hit;
	FCollisionQueryParams TraceParams;

	GetWorld()->LineTraceSingleByChannel(hit, Start, End, ECC_Visibility, TraceParams);*/

	
}


//////////////////////////////////////////////////////////////////////////
// Input

void AHalfLifeExperimentCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AHalfLifeExperimentCharacter::JumpJetPack);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AHalfLifeExperimentCharacter::OnJump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AHalfLifeExperimentCharacter::StopJump);


	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AHalfLifeExperimentCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AHalfLifeExperimentCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHalfLifeExperimentCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AHalfLifeExperimentCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AHalfLifeExperimentCharacter::LookUpAtRate);

	// Bind Dash Events
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AHalfLifeExperimentCharacter::MoveDash);

	// Bind Fire Event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AHalfLifeExperimentCharacter::OnFire);

	// Bind Interact Event
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AHalfLifeExperimentCharacter::OnInteract);

}

void AHalfLifeExperimentCharacter::OnFire()
{
	//Throw the object
	if (hasObject)
	{
		//Release the Object
		PhysicsHandle->ReleaseComponent();

		//Launch The Object
		FVector Forward = this->GetActorForwardVector();

		TheObject.GetComponent()->AddImpulse(FirstPersonCameraComponent->GetForwardVector()*FireForce,"",true);

		hasObject = false;
	}
	// grab the object
	else
	{
		FVector Start;
		FRotator rot;
		GetController()->GetPlayerViewPoint(Start, rot);
		FVector End = Start+(rot.Vector()*FireRange);

		FHitResult hit;
		FCollisionQueryParams TraceParams;

		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f);
		//if it's hit 
		if (GetWorld()->LineTraceSingleByChannel(hit, Start, End, ECC_Visibility, TraceParams))
			AHalfLifeExperimentCharacter::OnHolding(hit);
	}
}

void AHalfLifeExperimentCharacter::OnHolding(FHitResult hit)
{
	TheObject = hit;
	//if the component simulate the physics 
	if (hit.GetComponent()->IsSimulatingPhysics())
	{
		PhysicsHandle->GrabComponentAtLocation(hit.GetComponent(), "", hit.Location);
		hasObject = true;
	}
}


void AHalfLifeExperimentCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AHalfLifeExperimentCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AHalfLifeExperimentCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AHalfLifeExperimentCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
}

void AHalfLifeExperimentCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AHalfLifeExperimentCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AHalfLifeExperimentCharacter::MoveDash()
{
	// if player allowed to dash
	if (canDash)
	{
		//Get Dash Direction
		GetCharacterMovement()->BrakingFrictionFactor = 0.f;
		const FVector dashDirection = FVector(GetVelocity().X, GetVelocity().Y, 0).GetSafeNormal();

		//Push the player
		LaunchCharacter(dashDirection*DashDistance, true, true);
		canDash = false;

		//get the time for player to dash
		float DashTime = DashDistance / DashSpeed;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AHalfLifeExperimentCharacter::StopDashing, DashTime, false);
	}
}

void AHalfLifeExperimentCharacter::StopDashing()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->BrakingFrictionFactor = 2.f;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AHalfLifeExperimentCharacter::ResetDash, DashCooldown, false);
}

void AHalfLifeExperimentCharacter::ResetDash()
{
	canDash = true;
}

void AHalfLifeExperimentCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHalfLifeExperimentCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AHalfLifeExperimentCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AHalfLifeExperimentCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AHalfLifeExperimentCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AHalfLifeExperimentCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void AHalfLifeExperimentCharacter::SetPoint(int getPoint)
{
	Point += getPoint;
}

int AHalfLifeExperimentCharacter::getPoint()
{
	return Point;
}

void AHalfLifeExperimentCharacter::OnInteract()
{
	//TheObject.GetComponent()->AddImpulse(FirstPersonCameraComponent->GetForwardVector() * FireForce, "", true);
	
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = (FirstPersonCameraComponent->GetForwardVector() * InteractRange)+ FirstPersonCameraComponent->GetComponentLocation();

	FHitResult hit;
	FCollisionQueryParams TraceParams;

	//if it's hit 
	GetWorld()->LineTraceSingleByChannel(hit, Start, End, ECC_Visibility, TraceParams);

	if (hit.GetActor())
	{
		if (Cast<ADoorScript>(hit.Actor) != NULL)
		{
			ADoorScript* door = Cast<ADoorScript>(hit.Actor);

			door->OpenDoor(this);
		}
	}
}

void AHalfLifeExperimentCharacter::addItemList(FString item)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Got The Key"));
	itemList.Add(item);
}

FString AHalfLifeExperimentCharacter::GetItemList(int i)
{
	return itemList[i];
}

bool AHalfLifeExperimentCharacter::CheckItemFromList(FString item)
{
	return itemList.Contains(item);
}

void AHalfLifeExperimentCharacter::JumpJetPack()
{
	if (isTrust)
	{
		LaunchCharacter(GetActorUpVector() * Trust, false, false);
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AHalfLifeExperimentCharacter::JumpJetPack, DelayTimer, false);
	}
}

void AHalfLifeExperimentCharacter::OnJump()
{
	if (!GetCharacterMovement()->IsFalling())
		LaunchCharacter(GetActorUpVector() * JumpForce, false, false);
	else
	{
		isTrust = true;
		JumpJetPack();
	}
}

void AHalfLifeExperimentCharacter::StopJump()
{
	isTrust = false;
}

