#include "ProceduralAnimation/LegComponent.h"
#include "ProceduralAnimation/SplineLegComponent.h"
#include "ProceduralAnimation/LegStepperComponent.h"

ULegComponent::ULegComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	LegStepper = CreateDefaultSubobject<ULegStepperComponent>(TEXT("LegStepper"));
	for (int i = 0; i < NumOfSplines; ++i)
	{
		FName name(FString("SplineLeg_") + FString::FromInt(i));
		USplineLegComponent* splineLeg = CreateDefaultSubobject<USplineLegComponent>(name);
		SplineLegs.Add(splineLeg);
	}
}

void ULegComponent::Init(float MinAngle, float MaxAngle)
{
	LegStepper->UpdateTarget(true, MinAngle, MaxAngle);
	FVector target = LegStepper->GetTargetLocation();
	SplineLegs[ActiveLegIndex]->SetTargetLocation(target);
	SplineLegs[ActiveLegIndex]->SetIsLegActive(true);
}

void ULegComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULegComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (LegStepper->GetIsFarFromPoint())
	{
		LegStepper->UpdateTarget(true, -45, 45);
		FVector target = LegStepper->GetTargetLocation();
		SplineLegs[ActiveLegIndex]->SetIsLegActive(false);
		ActiveLegIndex = (ActiveLegIndex + 1) % 2;
		SplineLegs[ActiveLegIndex]->SetIsLegActive(true);
		SplineLegs[ActiveLegIndex]->SetTargetLocation(target);
	}
	FColor color = ActiveLegIndex ? FColor::Blue : FColor::Green;
	DrawDebugSphere(GetWorld(), LegStepper->GetTargetLocation(), 5.0f, 12, color, false, -1, 10, 2.5f);
}