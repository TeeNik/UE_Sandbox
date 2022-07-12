#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LegComponent.generated.h"


class USplineLegComponent;
class ULegStepperComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_SANDBOX_API ULegComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULegComponent();
	void Init(float MinAngle, float MaxAngle);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	int32 NumOfSplines = 2;

	UPROPERTY()
	TArray<USplineLegComponent*> SplineLegs;
	UPROPERTY()
	ULegStepperComponent* LegStepper;

	static bool PreviousLegSide;

private:
	int32 ActiveLegIndex = 0;
};
