// Fill out your copyright notice in the Description page of Project Settings.

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
	float StartScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline", Meta = (MakeEditWidget = true))
	float EndScale = 1.0f;
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

	float LegAnimationProgress = 1.0f;
	void PlayLegAnimation(float DeltaTime);
	bool IsLegHiding = false;

};
