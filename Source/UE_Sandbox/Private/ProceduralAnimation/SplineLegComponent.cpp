#include "ProceduralAnimation/SplineLegComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "ProceduralAnimation/LegStepperComponent.h"

USplineLegComponent::USplineLegComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	LegStepper = CreateDefaultSubobject<ULegStepperComponent>(TEXT("LegStepper"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshObj(TEXT("/Game/Creature/cylinder.cylinder"));
	StaticMesh = StaticMeshObj.Object;
}

void USplineLegComponent::SetIsLegActive(bool InIsLegActive)
{
	IsLegActive = InIsLegActive;
	if (IsLegActive)
	{
		LegStepper->GetNewPoint();
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

bool USplineLegComponent::IsLegShouldHide() const
{
	return LegStepper->GetIsFarFromPoint();
}

void USplineLegComponent::PlayReachAnimation()
{
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
	End = LegStepper->GetEndPoint();
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

	FVector current = GetOwner()->GetActorLocation();
	const int NumOfPoint = 10;
	const FTransform transform = GetOwner()->GetActorTransform();

	//const FVector start = transform.TransformPosition(Start);
	//const FVector control = transform.TransformPosition(Control);
	//const FVector end = transform.TransformPosition(End);

	const FVector start = transform.GetLocation();
	const FVector end = End;
	const FVector control = start + (end - start) / 2.0f + FVector::UpVector * ControlHeight;

	TArray<FVector> points;

	for (int i = 0; i <= NumOfPoint; ++i)
	{
		float value = i / (float)NumOfPoint * LegAnimationProgress;
		FVector worldPoint = GetBezierLocation(start, control, end, value);
		FVector localPoint = GetBezierLocation(Start, Control, End, value);
		points.Add(worldPoint);
		//DrawDebugLine(GetWorld(), current, worldPoint, FColor::Blue, false, 20, 0, 2.5);
		current = worldPoint;
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
	float value = 0.0f;

	if (IsPlayingReachAnimation)
	{
		value = DeltaTime;
	}
	else if (IsPlayingHideAnimation)
	{
		value = -DeltaTime;
	}

	LegAnimationProgress += value;
	if (LegAnimationProgress > 1 || LegAnimationProgress < 0)
	{
		IsPlayingReachAnimation = false;
		IsPlayingHideAnimation = false;
	}
	LegAnimationProgress = FMath::Clamp(LegAnimationProgress, 0, 1);

}

