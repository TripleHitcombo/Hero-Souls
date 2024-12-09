// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/LockonComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"	


// Sets default values for this component's properties
ULockOnComponent::ULockOnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
			
}


// Called when the game starts
void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerRef = GetOwner<ACharacter>();
	Controller = GetWorld()->GetFirstPlayerController();
	MovementComp = OwnerRef->GetCharacterMovement();
	SpringArmComp = OwnerRef->FindComponentByClass <USpringArmComponent>();
	
}

// Called every frame
void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!IsValid(CurrentTargetActor)) { return; }

	FVector CurrentLocation{ OwnerRef->GetActorLocation() };
	FVector TargetLocation{ CurrentTargetActor->GetActorLocation() };
    
	TargetLocation.Z -= 125;

	FRotator NewRotation{ UKismetMathLibrary::FindLookAtRotation 
	(CurrentLocation, TargetLocation) };
	
	Controller->SetControlRotation(NewRotation);

	double TargetDistance{ FVector::Distance(CurrentLocation, TargetLocation) };

	if (TargetDistance >= BreakDistance)
	{
		EndLockOn();
	}
	else
	{
		return;
	}
				
}
void ULockOnComponent::StartLockOn(float Radius)
{
	FHitResult OutResult;
	FVector	CurrentLocation { OwnerRef->GetActorLocation() };
	FCollisionShape Sphere{ FCollisionShape::MakeSphere(Radius) };
	FCollisionQueryParams IgnoreParams
	{
		FName { TEXT("Ignore Collision Parameters") },
		false,
		OwnerRef
	    
	};

	bool bHasFoundTarget{ GetWorld()->SweepSingleByChannel
	(
	 OutResult,
	 CurrentLocation,
	 CurrentLocation,
	 FQuat::Identity,
	 ECollisionChannel::ECC_GameTraceChannel1,
	 Sphere,
	 IgnoreParams
	) };

	if (!bHasFoundTarget)
	{
		return;
	}

	CurrentTargetActor = OutResult.GetActor();
	
	Controller->SetIgnoreLookInput(true);
	MovementComp->bOrientRotationToMovement = false;
	MovementComp->bUseControllerDesiredRotation = true;

	SpringArmComp->TargetOffset = FVector{ 0.0, 0.0, 100.0 };
}

void ULockOnComponent::EndLockOn()
{
	CurrentTargetActor = nullptr;

	MovementComp->bOrientRotationToMovement = true;
	MovementComp->bUseControllerDesiredRotation = false;
	SpringArmComp->TargetOffset = FVector::ZeroVector;

	Controller->ResetIgnoreLookInput();
}

void ULockOnComponent::ToggleLockOn(float Radius)
{
	if (IsValid(CurrentTargetActor))
	{
		EndLockOn();
	}
	else
	{
		StartLockOn();
	}
}




