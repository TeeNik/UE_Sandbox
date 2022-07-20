#include "ProceduralAnimation/LegStepperComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

ULegStepperComponent::ULegStepperComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	WantStepAtDistance = 200.0f;
	HomeTransform = FVector(0, 120, 0);
	StartEndPoint = HomeTransform;
	StepOvershootFraction = 0.5f;
	MinRadius = 175.0f;
	MaxRadius = 190.0f;
}

void ULegStepperComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULegStepperComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsFarFromPoint)
	{
		const FTransform ownerTransfrom = GetOwner()->GetTransform();
		const FVector worldHomePos = ownerTransfrom.GetLocation();
		const FVector up = GetOwner()->GetActorUpVector();

		const FVector homePos = FVector::VectorPlaneProject(worldHomePos, up);
		const FVector endPos = FVector::VectorPlaneProject(TargetPoint, up);
		//FVector diff = endPos - homePos;
		FVector diff = TargetPoint - worldHomePos;

		const float sqrDist = (diff).SquaredLength();
		//diff.Normalize();

		DrawDebugSphere(GetWorld(), homePos, 5.0f, 12, FColor::Blue, false, -1.0f, 10, 2.5f);
		DrawDebugSphere(GetWorld(), endPos, 5.0f, 12, FColor::Blue, false, -1.0f, 10, 2.5f);
		DrawDebugLine(GetWorld(), homePos, endPos, FColor::Blue, false, -1.0f, 10, 2.5f);

		int32 length = (int32)diff.Length();
		UKismetSystemLibrary::PrintString(GetWorld(), FString::FromInt(length), true, true, FColor::Blue, 0);


		if (sqrDist > WantStepAtDistance * WantStepAtDistance)
		{
			IsFarFromPoint = true;
		}
	}
}

FVector ULegStepperComponent::RaycastPointOnFloor(const FVector& Point) const
{
	FHitResult hit;
	FVector up = GetOwner()->GetActorUpVector();
	FVector rayOrigin = Point + up * 200.0f;
	FVector rayEnd = rayOrigin - up * 1000.0f;
	bool result = GetWorld()->LineTraceSingleByChannel(hit, rayOrigin, rayEnd, ECollisionChannel::ECC_WorldStatic);
	if (result)
	{
		return hit.ImpactPoint;
	}
	return Point;
}

FVector ULegStepperComponent::GetTargetLocation() const
{
	return TargetPoint;
}

void ULegStepperComponent::UpdateTarget(bool UseOwnerLocation, float MinAngle, float MaxAngle)
{
	const float angle = FMath::RandRange(MinAngle, MaxAngle);
	const FVector up = GetOwner()->GetActorUpVector();
	const FVector origin = UseOwnerLocation ? GetOwner()->GetActorLocation() : TargetPoint;
	FVector dir = UKismetMathLibrary::RotateAngleAxis(GetOwner()->GetActorForwardVector(), angle, up);
	dir.Normalize();
	UE_LOG(LogTemp, Log, TEXT("TargetNewPoint %f %s"), angle, *dir.ToString());
	const float radius = FMath::RandRange(MinRadius, MaxRadius);
	FVector point = origin + dir * radius;
	TargetPoint = RaycastPointOnFloor(point);
	IsFarFromPoint = false;
	//DrawDebugSphere(GetWorld(), TargetPoint, 5.0f, 12, FColor::Magenta, false, 30, 10, 2.5f);
}

bool ULegStepperComponent::GetIsFarFromPoint() const
{
	return IsFarFromPoint;
}

