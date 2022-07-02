#include "ProceduralAnimation/LegController.h"
#include "ProceduralAnimation/SplineLegComponent.h"

ULegController::ULegController()
{
	PrimaryComponentTick.bCanEverTick = true;

	for (int i = 0; i < NumOfLegs; ++i)
	{
		FName name(TEXT("Leg%d"), i);
		USplineLegComponent* leg = CreateDefaultSubobject<USplineLegComponent>(name);
		Legs.Add(leg);
	}
}

void ULegController::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < NumOfActiveLegs; ++i)
	{
		Legs[i]->SetIsLegActive(true);
	}
}

void ULegController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	int32 numOfLegsToActivate = 0;
	for (USplineLegComponent* leg : Legs)
	{
		if (leg->GetIsLegActive() && leg->IsLegShouldHide())
		{
			++numOfLegsToActivate;
			leg->SetIsLegActive(false);
		}
	}

	if (numOfLegsToActivate > 0)
	{
		for (USplineLegComponent* leg : Legs)
		{
			if (!leg->GetIsLegActive() && !leg->GetIsPlayingHideAnimation())
			{
				--numOfLegsToActivate;
				leg->SetIsLegActive(true);

				if (numOfLegsToActivate == 0) {
					break;
				}
			}
		}
	}
}