// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HERO_SOULS_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

	ACharacter* OwnerRef;

	APlayerController* Controller;

	class UCharacterMovementComponent* MovementComp;

	class USpringArmComponent* SpringArmComp;

public:	
	// Sets default values for this component's properties
	ULockOnComponent();

	AActor* CurrentTargetActor;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void StartLockOn(float Radius = 750.0f);

	void EndLockOn();

	UFUNCTION(BlueprintCallable)
	void ToggleLockOn(float Radius = 750.0f);

	double BreakDistance{ 1000.0 };

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
