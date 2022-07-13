#include "ProceduralAnimation/SplineLegComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "ProceduralAnimation/LegStepperComponent.h"

USplineLegComponent::USplineLegComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshObj(TEXT("/Game/Creature/cylinder.cylinder"));
	StaticMesh = StaticMeshObj.Object;

	for (int i = 0; i < NumOfPointsInSpline - 1; ++i)
	{
		PointOffsets.Add(FMath::RandRange(-5.0f, 5.0f));
	}
}

void USplineLegComponent::SetIsLegActive(bool InIsLegActive)
{
	IsLegActive = InIsLegActive;
	if (IsLegActive)
	{
		PlayReachAnimation();
	}
	else
	{
		PlayHideAnimation();
	}
}

bool USplineLegComponent::GetIsPlayingHideAnimation()
{
	return IsPlayingHideAnimation;
}

void USplineLegComponent::SetTargetLocation(const FVector& target)
{
	End = target;
}

void USplineLegComponent::PlayReachAnimation()
{
	LegIdleAnimation = 0.0f;
	LegAnimationProgress = 0.0f;
	IsPlayingReachAnimation = true;
}

void USplineLegComponent::PlayHideAnimation()
{
	LegAnimationProgress = 1.0f;
	IsPlayingHideAnimation = true;
}

void USplineLegComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USplineLegComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	PlayLegAnimation(DeltaTime);
	ConstructSpline();
}

FVector USplineLegComponent::GetBezierLocation(FVector start, FVector control, FVector end, float t) const
{
	FVector a = FMath::Lerp(start, control, t);
	FVector b = FMath::Lerp(control, end, t);
	return FMath::Lerp(a, b, t);
}

void USplineLegComponent::ConstructSpline()
{
	if (SplineMeshes.Num() > 0)
	{
		for (int i = 0; i < SplineMeshes.Num(); ++i)
		{
			if (IsValid(SplineMeshes[i]))
			{
				SplineMeshes[i]->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
				SplineMeshes[i]->DestroyComponent();
			}
		}
		SplineMeshes.Empty();
	}

	if (!IsLegActive && !IsPlayingHideAnimation)
	{
		return;
	}

	const FTransform transform = GetOwner()->GetActorTransform();

	const FVector start = transform.GetLocation();
	const FVector end = End;
	FVector control = start + (end - start) / 2.0f + FVector::UpVector * ControlHeight;
	//control += ControlOffset;

	TArray<FVector> points;

	for (int i = 0; i <= NumOfPointsInSpline; ++i)
	{
		float value = i / (float)NumOfPointsInSpline * LegAnimationProgress;
		FVector worldPoint = GetBezierLocation(start, control, end, value);

		//if (i > 0 && i < NumOfPointsInSpline)
		//{
		//	float offsetValue = PointOffsets[i - 1];
		//	FVector offset = FVector(0, offsetValue * FMath::Sin(LegIdleAnimation), offsetValue * FMath::Cos(LegIdleAnimation));
		//	worldPoint += offset * FMath::FloorToFloat(LegAnimationProgress);
		//}

		points.Add(worldPoint);
	}

	SplineComponent->SetSplinePoints(points, ESplineCoordinateSpace::Local, true);
	const int splinePoints = SplineComponent->GetNumberOfSplinePoints();

	for (int i = 0; i < splinePoints - 1; ++i)
	{
		USplineMeshComponent* splineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		splineMesh->SetStaticMesh(StaticMesh);
		splineMesh->SetForwardAxis(ESplineMeshAxis::X);
		splineMesh->SetMaterial(0, Material);

		splineMesh->RegisterComponentWithWorld(GetWorld());
		splineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		splineMesh->SetMobility(EComponentMobility::Movable);
		splineMesh->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

		const FVector startPoint = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector startTangent = SplineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector endPoint = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		const FVector endTangent = SplineComponent->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		const float startScale = FMath::Lerp(EndScale, StartScale, (splinePoints - i) / (float)splinePoints);
		const float endScale = FMath::Lerp(EndScale, StartScale, (splinePoints - (i + 1)) / (float)splinePoints);

		//const float startScale = StartScale * (splinePoints - i);
		//const float endScale = EndScale * (splinePoints - (i + 1));

		splineMesh->SetStartAndEnd(startPoint, startTangent, endPoint, endTangent);
		splineMesh->SetStartScale(FVector2D(startScale, startScale));
		splineMesh->SetEndScale(FVector2D(endScale, endScale));
		SplineMeshes.Add(splineMesh);
	}
}

void USplineLegComponent::PlayLegAnimation(float DeltaTime)
{
	LegIdleAnimation += DeltaTime;
	ControlOffset = FVector(0, 10 * FMath::Sin(LegIdleAnimation), 10 * FMath::Cos(LegIdleAnimation));

	if (!IsPlayingReachAnimation && !IsPlayingHideAnimation)
	{
		return;
	}

	float value = 0.0f;

	if (IsPlayingReachAnimation)
	{
		value = DeltaTime;
	}
	else if (IsPlayingHideAnimation)
	{
		value = -DeltaTime;
	}

	LegAnimationProgress += value * AnimationSpeed;
	if (LegAnimationProgress > 1 || LegAnimationProgress < 0)
	{
		IsPlayingReachAnimation = false;
		IsPlayingHideAnimation = false;
	}
	LegAnimationProgress = FMath::Clamp(LegAnimationProgress, 0, 1);

}

