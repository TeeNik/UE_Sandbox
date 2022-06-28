#include "ProceduralAnimation/SplineLegComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "ProceduralAnimation/LegStepperComponent.h"

USplineLegComponent::USplineLegComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void USplineLegComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USplineLegComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(LegStepper))
	{
		PlayLegAnimation(DeltaTime);

		End = LegStepper->GetEndPoint();
		ConstructSpline();
	}
	else
	{
		LegStepper = Cast<ULegStepperComponent>(GetOwner()->GetComponentByClass(ULegStepperComponent::StaticClass()));
	}
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
	//UE_LOG(LogTemp, Log, TEXT("DrawCurve %d"), splinePoints);

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

		splineMesh->SetStartAndEnd(startPoint, startTangent, endPoint, endTangent);
		splineMesh->SetStartScale(FVector2d(MeshScale, MeshScale));
		splineMesh->SetEndScale(FVector2d(MeshScale, MeshScale));
		SplineMeshes.Add(splineMesh);
	}
}

void USplineLegComponent::PlayLegAnimation(float DeltaTime)
{

	//float value = DeltaTime * 0.5;
	//if (IsLegHiding)
	//{
	//	value *= -1;
	//}
	//LegAnimationProgress += value;
	//if (LegAnimationProgress > 1 || LegAnimationProgress < 0)
	//{
	//	IsLegHiding = !IsLegHiding;
	//}
	//LegAnimationProgress = FMath::Clamp(LegAnimationProgress, 0, 1);

}

