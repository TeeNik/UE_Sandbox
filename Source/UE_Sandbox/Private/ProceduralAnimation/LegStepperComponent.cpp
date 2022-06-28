#include "ProceduralAnimation/LegStepperComponent.h"

ULegStepperComponent::ULegStepperComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	WantStepAtDistance = 120;
	HomeTransform = FVector(0, 120, 0);
	StartEndPoint = HomeTransform;
	StepOvershootFraction = 0.5f;
}

void ULegStepperComponent::BeginPlay()
{
	Super::BeginPlay();

	const FTransform ownerTransfrom = GetOwner()->GetTransform();
	EndPoint = ownerTransfrom.TransformPosition(StartEndPoint);
}

void ULegStepperComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const FTransform ownerTransfrom = GetOwner()->GetTransform();
	const FVector worldHomePos = ownerTransfrom.TransformPosition(HomeTransform);


	const FVector homePos = FVector::VectorPlaneProject(worldHomePos, FVector::UpVector);
	const FVector endPos = FVector::VectorPlaneProject(EndPoint, FVector::UpVector);
	FVector diff = endPos - homePos;

	const float sqrDist = (diff).SquaredLength();
	diff.Normalize();

	if(sqrDist > WantStepAtDistance * WantStepAtDistance)
	{
		const FVector forward = GetOwner()->GetActorForwardVector();
		const float overshootDistance = WantStepAtDistance * StepOvershootFraction;
		const FVector overshootVector = forward * overshootDistance;

		FHitResult hit;
		FVector rayOrigin = worldHomePos + overshootVector + FVector::UpVector * 200.0f;
		FVector rayEnd = rayOrigin - FVector::UpVector * 1000.0f;
		bool result = GetWorld()->LineTraceSingleByChannel(hit, rayOrigin, rayEnd, ECollisionChannel::ECC_WorldStatic);
		if (result)
		{
			DrawDebugSphere(GetWorld(), hit.ImpactPoint, 5.0f, 12, FColor::Yellow, false, 10, 10, 2.5f);
		}
		EndPoint = worldHomePos + overshootVector;
	}
	
	//DrawDebugLine(GetWorld(), endPos, homePos, FColor::Blue, false, -1, 10, 2.5f);
	DrawDebugLine(GetWorld(), endPos, homePos, FColor::Blue, false, -1, 10, 2.5f);
	DrawDebugSphere(GetWorld(), EndPoint, 5.0f, 12, FColor::Red, false, -1, 10, 2.5f);
	DrawDebugSphere(GetWorld(), worldHomePos, 5.0f, 12, FColor::Green, false, -1, 10, 2.5f);
}

FVector ULegStepperComponent::GetEndPoint() const
{
	FHitResult hit;
	FVector rayOrigin = EndPoint + FVector::UpVector * 200.0f;
	FVector rayEnd = rayOrigin - FVector::UpVector * 1000.0f;
	bool result = GetWorld()->LineTraceSingleByChannel(hit, rayOrigin, rayEnd, ECollisionChannel::ECC_WorldStatic);
	if (result)
	{
		return hit.ImpactPoint;
	}
	return EndPoint;
}

