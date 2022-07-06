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

	FVector GetTargetPoint() const;
	void TargetNewPoint(bool UseOwnerLocation, float MinAngle, float MaxAngle);
	void TargetNewPointInSegment(float MinAngle, float MaxAngle);
	void TargetNewPointInFront(float MinAngle, float MaxAngle);
	bool GetIsFarFromPoint() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper")
	float WantStepAtDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper")
	float StepOvershootFraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper")
	float MinRadius = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper")
	float MaxRadius = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper", Meta = (MakeEditWidget = true))
	FVector HomeTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper", Meta = (MakeEditWidget = true))
	FVector StartEndPoint;

private:
	FVector TargetPoint;
	FVector GetRandomPointInRadius(float MinAngle, float MaxAngle) const;
	FVector RaycastPointOnFloor(const FVector& Point) const;

	bool IsFarFromPoint = false;

};
