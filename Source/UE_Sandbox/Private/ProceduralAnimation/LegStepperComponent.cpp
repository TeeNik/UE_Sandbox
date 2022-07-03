#include "ProceduralAnimation/LegStepperComponent.h"
#include "Kismet/KismetMathLibrary.h"

ULegStepperComponent::ULegStepperComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	WantStepAtDistance = 300.0f;
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

	if (!IsFarFromPoint)
	{
		const FTransform ownerTransfrom = GetOwner()->GetTransform();
		const FVector worldHomePos = ownerTransfrom.TransformPosition(HomeTransform);

		const FVector homePos = FVector::VectorPlaneProject(worldHomePos, FVector::UpVector);
		const FVector endPos = FVector::VectorPlaneProject(EndPoint, FVector::UpVector);
		FVector diff = endPos - homePos;

		const float sqrDist = (diff).SquaredLength();
		diff.Normalize();

		if (sqrDist > WantStepAtDistance * WantStepAtDistance)
		{
			IsFarFromPoint = true;
		}
		DrawDebugSphere(GetWorld(), EndPoint, 5.0f, 12, FColor::Red, false, -1, 10, 2.5f);
	}
}

FVector ULegStepperComponent::GetRandomPointInRadius() const
{
	const float angle = FMath::RandRange(0.0f, 360.0f);
	const FVector up = GetOwner()->GetActorUpVector();
	FVector dir = GetOwner()->GetActorForwardVector();
	dir = UKismetMathLibrary::RotateAngleAxis(dir, angle, up);
	UE_LOG(LogTemp, Log, TEXT("GetRandomPointInRadius %f %s"), angle, *up.ToString());

	const float radius = FMath::RandRange(200, 300);

	FHitResult hit;
	FVector rayOrigin = GetOwner()->GetActorLocation() + dir * radius + FVector::UpVector * 200.0f;
	FVector rayEnd = rayOrigin - FVector::UpVector * 1000.0f;
	bool result = GetWorld()->LineTraceSingleByChannel(hit, rayOrigin, rayEnd, ECollisionChannel::ECC_WorldStatic);
	if (result)
	{
		DrawDebugSphere(GetWorld(), hit.ImpactPoint, 5.0f, 12, FColor::Magenta, false, 5, 10, 2.5f);
	}

	return hit.ImpactPoint;
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

void ULegStepperComponent::GetNewPoint()
{
	EndPoint = GetRandomPointInRadius();
	IsFarFromPoint = false;
}

bool ULegStepperComponent::GetIsFarFromPoint() const
{
	return IsFarFromPoint;
}

