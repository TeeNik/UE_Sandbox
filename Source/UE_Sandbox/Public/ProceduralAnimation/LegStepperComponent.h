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

	FVector GetEndPoint() const;

	void GetNewPoint();
	bool GetIsFarFromPoint() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper")
	float WantStepAtDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper")
	float StepOvershootFraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper", Meta = (MakeEditWidget = true))
	FVector HomeTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LegStepper", Meta = (MakeEditWidget = true))
	FVector StartEndPoint;

private:
	FVector EndPoint;
	FVector GetRandomPointInRadius() const;

	bool IsFarFromPoint = false;

};
