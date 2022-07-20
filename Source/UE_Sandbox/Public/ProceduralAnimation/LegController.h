#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LegController.generated.h"

class ULegComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_SANDBOX_API ULegController : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULegController();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, Category = "LegController")
	int32 NumOfLegs = 5;
	UPROPERTY(EditDefaultsOnly, Category = "LegController")
	float MinRadius = 200.0f;
	UPROPERTY(EditDefaultsOnly, Category = "LegController")
	float MaxRadius = 500.0f;

private:
	UPROPERTY(Transient)
	TArray<ULegComponent*> Legs;
	
};
