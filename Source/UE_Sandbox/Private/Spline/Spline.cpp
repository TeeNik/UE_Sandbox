// Fill out your copyright notice in the Description page of Project Settings.


#include "Spline/Spline.h"

ASpline::ASpline()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpline::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpline::DrawCurve()
{
	FVector current = GetActorLocation();
	const int NumOfPoint = 30;
	const FTransform transform = GetActorTransform();

	const FVector start = transform.TransformPosition(Start);
	const FVector control = transform.TransformPosition(Control);
	const FVector end = transform.TransformPosition(End);

	for (int i = 0; i <= NumOfPoint; ++i)
	{
		float value = i / (float)NumOfPoint;
		FVector point = GetBezierLocation(start, control, end, value);
		DrawDebugLine(GetWorld(), current, point, FColor::Blue, false, 20, 0, 2.5);
		current = point;
	}
	UE_LOG(LogTemp, Log, TEXT("DrawCurve"));
}

FVector ASpline::GetBezierLocation(FVector start, FVector control, FVector end, float t)
{
	FVector a = FMath::Lerp(start, control, t);
	FVector b = FMath::Lerp(control, end, t);
	return FMath::Lerp(a, b, t);
}