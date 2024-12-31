#include "Combat/TraceComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/DamageEvents.h"
#include "Interfaces/FighterInterface.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTraceComponent::UTraceComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame. 
    PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTraceComponent::BeginPlay()
{
    Super::BeginPlay();

    SkeletalComp = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    if (!SkeletalComp)
    {
        UE_LOG(LogTemp, Error, TEXT("SkeletalComp is null in TraceComponent::BeginPlay"));
    }
}

// Called every frame
void UTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsAttacking)
    {
        return;
    }

    TArray<FHitResult> AllResults;

    for (const FTraceSockets& Socket : Sockets)
    {
        FVector StartSocketLocation = SkeletalComp->GetSocketLocation(Socket.Start);
        FVector EndSocketLocation = SkeletalComp->GetSocketLocation(Socket.End);
        FQuat ShapeRotation = SkeletalComp->GetSocketQuaternion(Socket.Rotation);

        TArray<FHitResult> OutResults;
        double WeaponDistance = FVector::Distance(StartSocketLocation, EndSocketLocation);
        FVector BoxHalfExtent = FVector(BoxCollisionLength, BoxCollisionLength, WeaponDistance) / 2.0f;
        FCollisionShape Box = FCollisionShape::MakeBox(BoxHalfExtent);
        FCollisionQueryParams IgnoreParams(FName(TEXT("IgnoreParams")), false, GetOwner());

        bool bHasFoundTargets = GetWorld()->SweepMultiByChannel(
            OutResults,
            StartSocketLocation,
            EndSocketLocation,
            ShapeRotation,
            ECollisionChannel::ECC_GameTraceChannel1,
            Box,
            IgnoreParams
        );

        AllResults.Append(OutResults);

        if (bDebugMode)
        {
            FVector CentrePoint = UKismetMathLibrary::VLerp(StartSocketLocation, EndSocketLocation, 0.5f);
            UKismetSystemLibrary::DrawDebugBox(
                GetWorld(),
                CentrePoint,
                Box.GetExtent(),
                bHasFoundTargets ? FLinearColor::Green : FLinearColor::Red,
                ShapeRotation.Rotator(),
                1.0f,
                2.0f
            );
        }
    }

    if (AllResults.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No targets found during attack trace"));
        return;
    }

    float CharacterDamage = 0.0f;
    IFighterInterface* FighterRef = Cast<IFighterInterface>(GetOwner());
    if (FighterRef)
    {
        CharacterDamage = FighterRef->GetDamage();
    }

    FDamageEvent TargetAttackedEvent;

    for (const FHitResult& Hit : AllResults)
    {
        AActor* TargetActor = Hit.GetActor();
        if (TargetsToIgnore.Contains(TargetActor))
        {
            continue;
        }

        TargetActor->TakeDamage(
            CharacterDamage,
            TargetAttackedEvent,
            GetOwner()->GetInstigatorController(),
            GetOwner()
        );

        TargetsToIgnore.AddUnique(TargetActor);

        UGameplayStatics::SpawnEmitterAtLocation
        (
            GetWorld(),
            HitParticleTemplate,
            Hit.ImpactPoint
        );

    }
}

// Start attack logic
void UTraceComponent::BeginAttack()
{
    bIsAttacking = true;
    TargetsToIgnore.Empty(); // Clear the ignored targets at the start of each attack
    UE_LOG(LogTemp, Warning, TEXT("Attack started, bIsAttacking set to true"));
}

// End attack logic
void UTraceComponent::EndAttack()
{
    bIsAttacking = false;
    HandleResetAttack(); // Clear ignored targets
    UE_LOG(LogTemp, Warning, TEXT("Attack ended, bIsAttacking set to false"));
}

// Reset targets to ignore
void UTraceComponent::HandleResetAttack()
{
    TargetsToIgnore.Empty();
    UE_LOG(LogTemp, Warning, TEXT("Resetting TargetsToIgnore array"));
}

