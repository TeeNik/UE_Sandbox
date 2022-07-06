#include "ProceduralAnimation/LegComponent.h"
#include "ProceduralAnimation/SplineLegComponent.h"
#include "ProceduralAnimation/LegStepperComponent.h"

ULegComponent::ULegComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	LegStepper = CreateDefaultSubobject<ULegStepperComponent>(TEXT("LegStepper"));
	for (int i = 0; i < NumOfSplines; i += 2)
	{
		FName name(FString("SplineLeg_") + FString::FromInt(i));
		USplineLegComponent* splineLeg = CreateDefaultSubobject<USplineLegComponent>(name);
		SplineLegs.Add(splineLeg);
	}
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

	}
}

void ULegComponent::SwitchLegs()
{
}
