// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/TraceComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/DamageEvents.h"
#include "Interfaces/FighterInterface.h"

// Sets default values for this component's properties
UTraceComponent::UTraceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	SkeletalComp = GetOwner()
		->FindComponentByClass<USkeletalMeshComponent>();
}


// Called every frame
void UTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsAttacking)
	{
		return;
	}

	FVector StartSocketLocation{ SkeletalComp->GetSocketLocation(Start) };
	FVector EndSocketLocation{ SkeletalComp->GetSocketLocation(End) };
	FQuat ShapeRotation{ SkeletalComp->GetSocketQuaternion(Rotation) };

	TArray<FHitResult> OutResults;
	double WeapoDistance{
		FVector::Distance(StartSocketLocation, EndSocketLocation)
	};
	FVector BoxHalfExtent{
		BoxCollisionLength, BoxCollisionLength, WeapoDistance
	};
	BoxHalfExtent /= 2; // BoxHalfExtent = BoxHalfExtent / 2;
	FCollisionShape Box{
		FCollisionShape::MakeBox(BoxHalfExtent)
	};
	FCollisionQueryParams IgnoreParams{
		FName { TEXT("Ignore Params") },
		false,
		GetOwner()
	};

	bool bHasFoundTargets{ GetWorld()->SweepMultiByChannel(
		OutResults,
		StartSocketLocation,
		EndSocketLocation,
		ShapeRotation,
		ECollisionChannel::ECC_GameTraceChannel1,
		Box,
		IgnoreParams
	) };

	if (bDebugMode)
	{
		FVector CentrePoint //Calculate centre point of box using a linear interpolation
		{
			UKismetMathLibrary::VLerp
			(
				StartSocketLocation,
				EndSocketLocation, 0.5f
			)
		};
		
		UKismetSystemLibrary::DrawDebugBox
		(
			GetWorld(),
			CentrePoint,
			Box.GetExtent(),
			bHasFoundTargets ? FLinearColor::Green : FLinearColor::Red, //Ternary Operator to select value either red or green
			ShapeRotation.Rotator(),
			1.0f,
			2.0f
		);
	}

	if (OutResults.Num() == 0) 
	{
		return;
	}

	float CharacterDamage{ 0.0f };

	IFighterInterface* FighterRef{ Cast<IFighterInterface>(GetOwner()) };

	if (FighterRef)
	{
		CharacterDamage = FighterRef->GetDamage();
	}

	FDamageEvent TargetAttackedEvent; 

	for (const FHitResult& Hit: OutResults) //& asks for ref instead of new copy, much better performance
	{
		AActor* TargetActor { Hit.GetActor() };

		if (TargetsToIgnore.Contains(TargetActor))
		{
			continue; //use contunue to keep going instead of return which stops the flow
		}

		TargetActor->TakeDamage
		(
			CharacterDamage,
			TargetAttackedEvent,
			GetOwner()->GetInstigatorController(),
			GetOwner()
		  
		);

		TargetsToIgnore.AddUnique(TargetActor); //unique make sure value already exists and only adds new ones
	}
}

void UTraceComponent::HandleResetAttack()
{
	TargetsToIgnore.Empty();
}

