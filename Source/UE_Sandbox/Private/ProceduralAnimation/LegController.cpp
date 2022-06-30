#include "ProceduralAnimation/LegController.h"
#include "ProceduralAnimation/SplineLegComponent.h"

ULegController::ULegController()
{
	PrimaryComponentTick.bCanEverTick = true;

	for (int i = 0; i < NumOfLegs; ++i)
	{
		FName name(TEXT("Leg%d"), i);
		USplineLegComponent* leg = CreateDefaultSubobject<USplineLegComponent>(name);
		Legs.Add(leg);
		if (i < NumOfActiveLegs)
		{
			leg->SetActive(true);
		}
	}
}

void ULegController::BeginPlay()
{
	Super::BeginPlay();

	//DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), MinRadius, 32, FColor::Magenta, false, 15, 10, 2.5f);
	//DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), MaxRadius, 32, FColor::Cyan, false, 15, 10, 2.5f);
}

void ULegController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	time -= DeltaTime;
	if (time < 0)
	{
		time = 5;
		GetRandomPointInRadius();
	}

}

FVector ULegController::GetRandomPointInRadius() const
{
	FVector2D dir2D = FMath::RandPointInCircle(1.0f);
	FVector dir(dir2D.X, .0f, dir2D.Y);
	dir.Normalize();
	const float radius = FMath::RandRange(MinRadius, MaxRadius);

	FHitResult hit;
	FVector rayOrigin = GetOwner()->GetActorLocation() + dir * radius + FVector::UpVector * 200.0f;
	FVector rayEnd = rayOrigin - FVector::UpVector * 1000.0f;
	bool result = GetWorld()->LineTraceSingleByChannel(hit, rayOrigin, rayEnd, ECollisionChannel::ECC_WorldStatic);
	if (result)
	{
		DrawDebugSphere(GetWorld(), hit.ImpactPoint, 5.0f, 12, FColor::Magenta, false, 5, 10, 2.5f);
	}

	return hit.ImpactPoint;
}
