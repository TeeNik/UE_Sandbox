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

	TargetPoint = GetActorLocation();
}

void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForwardSurface();
	UpdateHeight(DeltaTime);

	//TraceMovement();
	//CalculateMovement();
	//Move(DeltaTime);
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
		
		FRotator newRot = FMath::Lerp(GetActorRotation(), TargetRotation, 0.1f);
		
		SetActorRotation(TargetRotation);
		SetActorLocation(current + Forward * Value * speed);
		//AddMovementInput(Direction, Value);
	}
	MoveForwardValue = Value;
}

void ACreature::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		//const FRotator Rotation = Controller->GetControlRotation();
		//const FRotator YawRotation(0, Rotation.Yaw, 0);
		//
		//// get right vector 
		//const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		//AddMovementInput(Direction, Value);
	}
	MoveRightValue = Value;
}

void ACreature::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACreature::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACreature::CheckForwardSurface()
{
	FHitResult hit1;
	RaycastForwardSurface(100, 50, hit1);
	FHitResult hit2;
	RaycastForwardSurface(120, 50, hit2);

	TargetPoint = hit1.ImpactPoint + hit1.ImpactNormal * BaseHeight;
	FVector newForward = hit2.ImpactPoint - hit1.ImpactPoint;
	newForward.Normalize();
	Forward = newForward;
	TargetRotation = UKismetMathLibrary::MakeRotFromXZ(newForward, hit1.ImpactNormal);
	//TargetRotation = UKismetMathLibrary::MakeRotFromX(newForward);
	
	FVector newUp = FVector::CrossProduct(newForward, GetActorRightVector());
	//FVector newRight = FVector::CrossProduct(hit1.ImpactNormal, newForward);
	FVector newRight = GetActorRightVector();

	FVector origin = GetActorLocation();
	DrawDebugLine(GetWorld(), origin, origin + newForward * 50,
		FColor::Red, false, -1, 20, 2);
	DrawDebugLine(GetWorld(), origin, origin + newUp * 50,
		FColor::Blue, false, -1, 20, 2);
	DrawDebugLine(GetWorld(), origin, origin + newRight * 50,
		FColor::Green, false, -1, 20, 2);
	//FVector newForward = FVector::CrossProduct(actorRight, OutHitResult.ImpactNormal);
	//TargetRotation = UKismetMathLibrary::MakeRotFromXZ(newForward, OutHitResult.ImpactNormal);

	TargetRotation = UKismetMathLibrary::MakeRotationFromAxes(newForward, 
		newRight, newUp);

}

bool ACreature::RaycastForwardSurface(float RaycastHeight, float RaycastForwardDist, FHitResult& OutHitResult)
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

		bool result = GetWorld()->LineTraceSingleByChannel(OutHitResult, upPos, forwardPos, ECollisionChannel::ECC_WorldStatic);
		if (result)
		{
			DrawDebugSphere(GetWorld(), OutHitResult.ImpactPoint, 5.0f, 12, FColor::Magenta, false, -1, 10, 2.5f);

			//TargetPoint = OutHitResult.ImpactPoint + OutHitResult.ImpactNormal * BaseHeight;
			//FVector newForward = FVector::CrossProduct(actorRight, OutHitResult.ImpactNormal);
			//TargetRotation = UKismetMathLibrary::MakeRotFromXZ(newForward, OutHitResult.ImpactNormal);

			DrawDebugSphere(GetWorld(), TargetPoint, 5.0f, 12, FColor::Purple, false, -1, 10, 2.5f);
			return result;
		}
	}

	return false;
}

void ACreature::UpdateHeight(float DeltaTime)
{
	const FVector origin = GetActorLocation();
	const FVector actorUp = GetActorUpVector();
	FHitResult hit;
	FVector end = origin - actorUp * 1000;
	bool result = GetWorld()->LineTraceSingleByChannel(hit, origin, end, ECollisionChannel::ECC_WorldStatic);
	if (result)
	{
		float height = FVector::Dist(origin, hit.ImpactPoint);
		float diff = BaseHeight - height;
		//if (diff > 5.0f)
		{
			FVector targetHeight = hit.ImpactPoint + actorUp * BaseHeight;
			DrawDebugSphere(GetWorld(), targetHeight, 5.0f, 12,
				FColor::Silver, false, -1, 10, 2.5f);
			FVector newPos = FMath::Lerp(origin, targetHeight, 50.0f * DeltaTime);

			SetActorLocation(targetHeight);
		}
	}
}

void ACreature::TraceMovement()
{
	TraceResult = false;
	FHitResult hit;
	const FVector origin = GetActorLocation();
	const FVector actorForward = GetActorForwardVector();
	const FVector actorRight = GetActorRightVector();
	const FVector actorUp = GetActorUpVector();
	const FVector moveDir = GetMovementDir();

	FVector end = origin + moveDir * 1000 - actorUp * 500;

	bool result = GetWorld()->LineTraceSingleByChannel(hit, origin, end, ECollisionChannel::ECC_WorldStatic);
	if (!result)
	{
		return;
	}

	F1 = hit;
	DrawDebugLine(GetWorld(), origin, end, FColor::Red, false, -1, 0, 2);

	FVector end2 = origin + moveDir * 1100 - actorUp * 500;
	bool result2 = GetWorld()->LineTraceSingleByChannel(hit, origin, end, ECollisionChannel::ECC_WorldStatic);
	if (!result2)
	{
		return;
	}
	F2 = hit;
	DrawDebugLine(GetWorld(), origin, end2, FColor::Red, false, -1, 0, 2);

	FVector endRight = UKismetMathLibrary::RotateAngleAxis(moveDir, 90, actorUp);
	FVector end3 = origin + moveDir * 1000 - actorUp * 500 + endRight * 100;
	bool result3 = GetWorld()->LineTraceSingleByChannel(hit, origin, end, ECollisionChannel::ECC_WorldStatic);
	if (!result3)
	{
		return;
	}
	R = hit;
	DrawDebugLine(GetWorld(), origin, end3, FColor::Red, false, -1, 0, 2);
	TraceResult = true;
}

void ACreature::CalculateMovement()
{
	if (TraceResult)
	{
		TargetPoint = F1.ImpactPoint + F1.ImpactNormal * BaseHeight;

		FVector newForward = UKismetMathLibrary::GetDirectionUnitVector(F1.ImpactPoint, F2.ImpactPoint);
		FVector newRight = UKismetMathLibrary::GetDirectionUnitVector(F1.ImpactPoint, R.ImpactPoint);
		TargetRotation = UKismetMathLibrary::MakeRotationFromAxes(newForward, newRight, F1.ImpactNormal);
		DrawDebugSphere(GetWorld(), TargetPoint, 5.0f, 12, FColor::Magenta, false, -1, 10, 2.5f);
	}
}

void ACreature::Move(float DeltaTime)
{
	const float moveValue = FMath::Abs(MoveForwardValue) + FMath::Abs(MoveRightValue);
	const FVector origin = GetActorLocation();
	const float dist = FVector::Distance(origin, TargetPoint);
	if (moveValue > 0.0f && dist > 2.0f)
	{
		const FVector dir = UKismetMathLibrary::GetDirectionUnitVector(origin, TargetPoint);
		
		SetActorLocation(origin + dir * Speed * DeltaTime);
	}
}

FVector ACreature::GetMovementDir() const
{
	FVector dir = GetActorForwardVector() * MoveForwardValue + GetActorRightVector() * MoveRightValue;
	dir.Normalize();
	return dir;
}
