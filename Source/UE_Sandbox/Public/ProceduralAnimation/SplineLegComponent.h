#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SplineLegComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_SANDBOX_API USplineLegComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	USplineLegComponent();

	void Init(float MinAngle, float MaxAngle);
	void SetIsLegActive(bool InIsLegActive);
	FORCEINLINE bool GetIsLegActive() { return IsLegActive; }
	bool GetIsPlayingHideAnimation();
	bool IsLegShouldHide() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Curve)
	FVector Start;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Curve, Meta = (MakeEditWidget = true))
	FVector Control;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Curve, Meta = (MakeEditWidget = true))
	FVector End;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline", Meta = (MakeEditWidget = true))
	float StartScale = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline", Meta = (MakeEditWidget = true))
	float EndScale = 0.025f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline", Meta = (MakeEditWidget = true))
	float MeshScale = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline", Meta = (MakeEditWidget = true))
	float ControlHeight = 80.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spline")
	UStaticMesh* StaticMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Spline")
	UMaterialInterface* Material;

private:	
	FVector GetBezierLocation(FVector start, FVector control, FVector end, float t) const;
	void ConstructSpline();
	UPROPERTY()
	TArray<class USplineMeshComponent*> SplineMeshes;
	UPROPERTY()
	class ULegStepperComponent* LegStepper;
	UPROPERTY()
	class USplineComponent* SplineComponent;

	void PlayLegAnimation(float DeltaTime);
	void PlayReachAnimation();
	void PlayHideAnimation();

	bool IsPlayingHideAnimation = false;
	bool IsPlayingReachAnimation = false;
	bool IsLegActive = false;
	float LegAnimationProgress = 1.0f;

};