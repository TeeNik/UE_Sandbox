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

	void SetIsLegActive(bool InIsLegActive);
	FORCEINLINE bool GetIsLegActive() { return IsLegActive; }
	bool GetIsPlayingHideAnimation();

	void SetTargetLocation(const FVector& target);


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
	float ControlHeight = 120.0f;

	UPROPERTY()
	float AnimationSpeed = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spline")
	UStaticMesh* StaticMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Spline")
	UMaterialInterface* Material;

private:	
	FVector GetBezierLocation(FVector start, FVector control, FVector end, float t) const;
	void ConstructSpline();
	UPROPERTY(Transient)
	TArray<class USplineMeshComponent*> SplineMeshes;
	UPROPERTY(Transient)
	class USplineComponent* SplineComponent;
	UPROPERTY(Transient)
	TArray<float> PointOffsets;
	UPROPERTY()
	int32 NumOfPointsInSpline = 2;


	void PlayLegAnimation(float DeltaTime);
	void PlayReachAnimation();
	void PlayHideAnimation();

	bool IsPlayingHideAnimation = false;
	bool IsPlayingReachAnimation = false;
	bool IsLegActive = false;
	float LegAnimationProgress = 1.0f;

	float LegIdleAnimation = 0.0f;
	FVector ControlOffset;

};