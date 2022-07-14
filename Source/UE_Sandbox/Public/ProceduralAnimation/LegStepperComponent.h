#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LegStepperComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_SANDBOX_API ULegStepperComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	ULegStepperComponent();

	FVector GetTargetLocation() const;
	void UpdateTarget(bool UseOwnerLocation, float MinAngle, float MaxAngle);
	bool GetIsFarFromPoint() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper")
	float WantStepAtDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper")
	float StepOvershootFraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper")
	float MinRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper")
	float MaxRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper", Meta = (MakeEditWidget = true))
	FVector HomeTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper", Meta = (MakeEditWidget = true))
	FVector StartEndPoint;

private:
	FVector TargetPoint;
	FVector RaycastPointOnFloor(const FVector& Point) const;

	bool IsFarFromPoint = false;

};
