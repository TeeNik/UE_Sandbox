#include "ProceduralAnimation/Creature.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACreature::ACreature()
{
	PrimaryActorTick.bCanEverTick = true;

	TurnRateGamepad = 50.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	//SetRootComponent(CapsuleComponent);

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void ACreature::BeginPlay()
{
	Super::BeginPlay();
}

void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForwardSurface();
}

void ACreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ACreature::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ACreature::MoveRight);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ACreature::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ACreature::LookUpAtRate);
}

void ACreature::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const float speed = 5.0f;
		FVector current = GetActorLocation();
		FVector dir = TargetPoint - current;
		dir.Normalize();

		FRotator lookRot = UKismetMathLibrary::FindLookAtRotation(current, TargetPoint);

		FRotator newRot = FMath::Lerp(GetActorRotation(), TargetRotation, 0.1f);

		SetActorRotation(newRot);
		SetActorLocation(current + dir * Value * speed);

		//AddMovementInput(Direction, Value);
	}
}

void ACreature::MoveRight(float Value)
{
	return;

	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ACreature::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACreature::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

FVector ACreature::CheckForwardSurface()
{
	const float angle = 270.0f;
	const float step = 15.0f;

	const float RaycastHeight = 100.0f;
	const float RaycastForwardDist = 50.0f;

	const FVector origin = GetActorLocation();
	const FVector actorUp = GetActorUpVector();
	const FVector actorForward = GetActorForwardVector();
	const FVector actorRight = GetActorRightVector();

	//DrawDebugLine(GetWorld(), origin, origin + actorForward * RaycastHeight, FColor::Red, false, -1, 0, 2);
	//DrawDebugLine(GetWorld(), origin, origin + actorUp * RaycastHeight, FColor::Blue, false, -1, 0, 2);
	//DrawDebugLine(GetWorld(), origin, origin + actorRight * RaycastHeight, FColor::Green, false, -1, 0, 2);

	for (int i = 0; i <= angle / step; ++i)
	{
		FVector up = UKismetMathLibrary::RotateAngleAxis(actorUp, step * i, actorRight);
		FVector upPos = origin + up * RaycastHeight;
		FVector forward = UKismetMathLibrary::RotateAngleAxis(actorForward, step * i, actorRight);
		FVector forwardPos = upPos + forward * RaycastForwardDist;

		DrawDebugLine(GetWorld(), origin, upPos, FColor::Green, false, -1, 0, 2);
		DrawDebugLine(GetWorld(), upPos, forwardPos, FColor::Green, false, -1, 0, 2);

		FHitResult hit;
		bool result = GetWorld()->LineTraceSingleByChannel(hit, upPos, forwardPos, ECollisionChannel::ECC_WorldStatic);
		if (result)
		{	
			DrawDebugSphere(GetWorld(), hit.ImpactPoint, 5.0f, 12, FColor::Magenta, false, -1, 10, 2.5f);
			
			TargetPoint = hit.ImpactPoint + hit.ImpactNormal * BaseHeight;
			
			FVector newForward = FVector::CrossProduct(actorRight, hit.ImpactNormal);
			TargetRotation = UKismetMathLibrary::MakeRotFromXZ(newForward, hit.ImpactNormal);

			DrawDebugSphere(GetWorld(), TargetPoint, 5.0f, 12, FColor::Purple, false, -1, 10, 2.5f);

			return hit.ImpactPoint;
		}
	}

	return origin;
}

FHitResult ACreature::RaycastForwardSurface(float RaycastHeight, float RaycastForwardDist)
{
	const float angle = 270.0f;
	const float step = 15.0f;

	const FVector origin = GetActorLocation();
	const FVector actorUp = GetActorUpVector();
	const FVector actorForward = GetActorForwardVector();
	const FVector actorRight = GetActorRightVector();

	for (int i = 0; i <= angle / step; ++i)
	{
		FVector up = UKismetMathLibrary::RotateAngleAxis(actorUp, step * i, actorRight);
		FVector upPos = origin + up * RaycastHeight;
		FVector forward = UKismetMathLibrary::RotateAngleAxis(actorForward, step * i, actorRight);
		FVector forwardPos = upPos + forward * RaycastForwardDist;

		DrawDebugLine(GetWorld(), origin, upPos, FColor::Green, false, -1, 0, 2);
		DrawDebugLine(GetWorld(), upPos, forwardPos, FColor::Green, false, -1, 0, 2);

		FHitResult hit;
		bool result = GetWorld()->LineTraceSingleByChannel(hit, upPos, forwardPos, ECollisionChannel::ECC_WorldStatic);
		if (result)
		{
			DrawDebugSphere(GetWorld(), hit.ImpactPoint, 5.0f, 12, FColor::Magenta, false, -1, 10, 2.5f);

			TargetPoint = hit.ImpactPoint + hit.ImpactNormal * BaseHeight;

			FVector newForward = FVector::CrossProduct(actorRight, hit.ImpactNormal);
			TargetRotation = UKismetMathLibrary::MakeRotFromXZ(newForward, hit.ImpactNormal);

			DrawDebugSphere(GetWorld(), TargetPoint, 5.0f, 12, FColor::Purple, false, -1, 10, 2.5f);
			return hit;
		}
	}

	return FHitResult();
}
