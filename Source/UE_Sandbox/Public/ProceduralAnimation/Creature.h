// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Creature.generated.h"

UCLASS()
class UE_SANDBOX_API ACreature : public ACharacter
{
	GENERATED_BODY()

public:
	ACreature();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Speed = 200.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float BaseHeight = 50.0f;

	void CheckForwardSurface();
	bool RaycastForwardSurface(float RaycastHeight, float RaycastForwardDist, FHitResult& OutHitResult);


	void Trace();
	void TraceMovement();
	void CalculateMovement();
	void Move(float DeltaTime);
	FVector GetMovementDir() const;

private:
	FVector TargetPoint;
	FRotator TargetRotation;
	FVector Forward;

	float MoveForwardValue;
	float MoveRightValue;

	FHitResult F1;
	FHitResult F2;
	FHitResult R;
	bool TraceResult;
	
};
