#include "ProceduralAnimation/LegStepperComponent.h"

ULegStepperComponent::ULegStepperComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULegStepperComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULegStepperComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const FTransform ownerTransfrom = GetOwner()->GetTransform();
	const FVector worldEndPos = ownerTransfrom.TransformPosition(EndPoint);
	const FVector worldHomePos = ownerTransfrom.TransformPosition(HomeTransform);

	const FVector homePos = FVector::VectorPlaneProject(worldHomePos, FVector::UpVector);
	const FVector endPos = FVector::VectorPlaneProject(worldEndPos, FVector::UpVector);

	const float sqrDist = (endPos - homePos).SquaredLength();

	if(sqrDist > WantStepAtDistance * WantStepAtDistance)
	{
		EndPoint = HomeTransform;
	}
	
	//DrawDebugLine(GetWorld(), endPos, homePos, FColor::Blue, false, -1, 10, 2.5f);
	DrawDebugLine(GetWorld(), endPos, ownerTransfrom.GetLocation(), FColor::Blue, false, -1, 10, 2.5f);
	DrawDebugSphere(GetWorld(), worldEndPos, 5.0f, 12, FColor::Red, false, -1, 10, 2.5f);
	DrawDebugSphere(GetWorld(), worldHomePos, 5.0f, 12, FColor::Green, false, -1, 10, 2.5f);
}

