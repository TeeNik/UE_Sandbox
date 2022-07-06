#include "ProceduralAnimation/LegStepperComponent.h"
#include "Kismet/KismetMathLibrary.h"

ULegStepperComponent::ULegStepperComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	WantStepAtDistance = 230.0f;
	HomeTransform = FVector(0, 120, 0);
	StartEndPoint = HomeTransform;
	StepOvershootFraction = 0.5f;
}

void ULegStepperComponent::BeginPlay()
{
	Super::BeginPlay();

	const FTransform ownerTransfrom = GetOwner()->GetTransform();
	TargetPoint = ownerTransfrom.TransformPosition(StartEndPoint);
}

void ULegStepperComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsFarFromPoint)
	{
		const FTransform ownerTransfrom = GetOwner()->GetTransform();
		const FVector worldHomePos = ownerTransfrom.TransformPosition(HomeTransform);

		const FVector homePos = FVector::VectorPlaneProject(worldHomePos, FVector::UpVector);
		const FVector endPos = FVector::VectorPlaneProject(TargetPoint, FVector::UpVector);
		FVector diff = endPos - homePos;

		const float sqrDist = (diff).SquaredLength();
		diff.Normalize();

		if (sqrDist > WantStepAtDistance * WantStepAtDistance)
		{
			IsFarFromPoint = true;
		}
		DrawDebugSphere(GetWorld(), TargetPoint, 5.0f, 12, FColor::Red, false, -1, 10, 2.5f);
	}
}

FVector ULegStepperComponent::GetRandomPointInRadius(float MinAngle, float MaxAngle) const
{
	const float angle = FMath::RandRange(MinAngle, MaxAngle);
	const FVector up = GetOwner()->GetActorUpVector();
	FVector dir = GetOwner()->GetActorForwardVector();
	dir = UKismetMathLibrary::RotateAngleAxis(dir, angle, up);
	UE_LOG(LogTemp, Log, TEXT("GetRandomPointInRadius %f %s"), angle, *up.ToString());

	const float radius = FMath::RandRange(MinRadius, MaxRadius);
	return RaycastPointOnFloor(GetOwner()->GetActorLocation() + dir * radius);
}

FVector ULegStepperComponent::RaycastPointOnFloor(const FVector& Point) const
{
	FHitResult hit;
	FVector rayOrigin = Point + FVector::UpVector * 200.0f;
	FVector rayEnd = rayOrigin - FVector::UpVector * 1000.0f;
	bool result = GetWorld()->LineTraceSingleByChannel(hit, rayOrigin, rayEnd, ECollisionChannel::ECC_WorldStatic);
	if (result)
	{
		return hit.ImpactPoint;
	}
	return Point;
}

FVector ULegStepperComponent::GetTargetPoint() const
{
	return TargetPoint;
}

void ULegStepperComponent::TargetNewPoint(bool UseOwnerLocation, float MinAngle, float MaxAngle)
{
	const float angle = FMath::RandRange(MinAngle, MaxAngle);
	const FVector up = GetOwner()->GetActorUpVector();
	const FVector origin = UseOwnerLocation ? GetOwner()->GetActorLocation() : TargetPoint;
	const FVector dir = UKismetMathLibrary::RotateAngleAxis(GetOwner()->GetActorForwardVector(), angle, up);
	UE_LOG(LogTemp, Log, TEXT("TargetNewPoint %f %s"), angle, *dir.ToString());
	DrawDebugLine(GetWorld(), origin, origin + dir * 100, FColor::Blue, false, 20, 0, 1.5f);
	const float radius = FMath::RandRange(MinRadius, MaxRadius);
	FVector point = origin + dir * radius;
	TargetPoint = RaycastPointOnFloor(point);
}

void ULegStepperComponent::TargetNewPointInSegment(float MinAngle, float MaxAngle)
{
	TargetPoint = GetRandomPointInRadius(MinAngle, MaxAngle);
	IsFarFromPoint = false;
}

void ULegStepperComponent::TargetNewPointInFront(float MinAngle, float MaxAngle)
{
	const float angle = FMath::RandRange(MinAngle, MaxAngle);
	const FVector up = GetOwner()->GetActorUpVector();
	FVector dir = GetOwner()->GetActorForwardVector();
	dir = UKismetMathLibrary::RotateAngleAxis(dir, angle, up);
	UE_LOG(LogTemp, Log, TEXT("GetRandomPointInRadius %f %s"), angle, *up.ToString());

}

bool ULegStepperComponent::GetIsFarFromPoint() const
{
	return IsFarFromPoint;
}

