#include "ProceduralAnimation/LegController.h"
#include "ProceduralAnimation/SplineLegComponent.h"

ULegController::ULegController()
{
	PrimaryComponentTick.bCanEverTick = true;

	for (int i = 0; i < NumOfLegs; ++i)
	{
		FName name(FString("Leg_") +  FString::FromInt(i));
		USplineLegComponent* leg = CreateDefaultSubobject<USplineLegComponent>(name);
		Legs.Add(leg);
	}
}

void ULegController::BeginPlay()
{
	Super::BeginPlay();

	float angle = 0.0f;
	float anglePeriod = 360.0f / (NumOfLegs / 2);

	for (int i = 0; i < NumOfLegs; i += 2)
	{
		//Legs[i]->SetIsLegActive(true);
		Legs[i]->Init(angle, angle + anglePeriod);
		angle += anglePeriod;
	}
}

void ULegController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int i = 0; i < NumOfLegs; ++i)
	{
		USplineLegComponent* leg = Legs[i];
		if(leg->GetIsLegActive() && leg->IsLegShouldHide())
	}

	for (USplineLegComponent* leg : Legs)
	{
		if (leg->GetIsLegActive() && leg->IsLegShouldHide())
		{

			leg->SetIsLegActive(false);
		}
	}
}

USplineLegComponent* ULegController::GetLegPair(int32 index)
{
	if (index % 2 == 0)
		return Legs[index + 1];
	else
		return Legs[index - 1];
}
