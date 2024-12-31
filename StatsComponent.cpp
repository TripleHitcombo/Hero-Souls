#include "Characters/StatsComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/FighterInterface.h"

UStatsComponent::UStatsComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    UE_LOG(LogTemp, Warning, TEXT("StatsComponent Constructor called."));
}

void UStatsComponent::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("StatsComponent BeginPlay called."));

    if (!Stats.Contains(EStat::Health)) Stats.Add(EStat::Health, 100.0f);
    if (!Stats.Contains(EStat::MaxHealth)) Stats.Add(EStat::MaxHealth, 100.0f);
    if (!Stats.Contains(EStat::Stamina)) Stats.Add(EStat::Stamina, 50.0f);
    if (!Stats.Contains(EStat::MaxStamina)) Stats.Add(EStat::MaxStamina, 50.0f);

    UE_LOG(LogTemp, Warning, TEXT("Stats initialized: Health=%f, Stamina=%f"),
        Stats[EStat::Health], Stats[EStat::Stamina]);
}

void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Debug: RegenStamina temporarily disabled
    RegenStamina();
}


void UStatsComponent::ReduceHealth(float Amount, AActor* Opponent)
{
    UE_LOG(LogTemp, Warning, TEXT("ReduceHealth called with Amount=%f"), Amount);

    if (!Stats.Contains(EStat::Health) || !Stats.Contains(EStat::MaxHealth))
    {
        UE_LOG(LogTemp, Error, TEXT("Health keys missing in Stats map"));
        return;
    }

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        UE_LOG(LogTemp, Error, TEXT("Owner is null in ReduceHealth"));
        return;
    }

    IFighterInterface* FighterRef = Cast<IFighterInterface>(Owner);
    if (!FighterRef || !FighterRef->CanTakeDamage(Opponent))
    {
        UE_LOG(LogTemp, Warning, TEXT("FighterRef is invalid or cannot take damage"));
        return;
    }

    Stats[EStat::Health] -= Amount;
    Stats[EStat::Health] = UKismetMathLibrary::FClamp(
        Stats[EStat::Health],
        0,
        Stats[EStat::MaxHealth]
    );

    UE_LOG(LogTemp, Warning, TEXT("Health reduced to: %f"), Stats[EStat::Health]);

    OnHealthPercentUpdateDelegate.Broadcast(
        GetStatPercentage(EStat::Health, EStat::MaxHealth)
    );

    if (Stats[EStat::Health] <= 0)
    {
        OnZeroHealthDelegate.Broadcast();
    }
}

void UStatsComponent::ReduceStamina(float Amount)
{
    if (!Stats.Contains(EStat::Stamina) || !Stats.Contains(EStat::MaxStamina))
    {
        UE_LOG(LogTemp, Error, TEXT("Stats map is missing keys for Stamina or MaxStamina!"));
        return;
    }

    if (!GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("GetWorld() is null in ReduceStamina!"));
        return;
    }

    Stats[EStat::Stamina] -= Amount;
    Stats[EStat::Stamina] = UKismetMathLibrary::FClamp(
        Stats[EStat::Stamina],
        0,
        Stats[EStat::MaxStamina]
    );

     bCanRegen = false;

    FLatentActionInfo FunctionInfo{
        0,
        100,
        TEXT("EnableRegen"),
        this
    };

    UKismetSystemLibrary::RetriggerableDelay(
        GetWorld(),
        StaminaDelayDuration,
        FunctionInfo
    );

    OnStaminaPercentUpdateDelegate.Broadcast
    (
        GetStatPercentage(EStat::Stamina, EStat::MaxStamina)
    );
}

void UStatsComponent::RegenStamina()
{
    
    if (!bCanRegen)
    {
        
        return;
    }

   
    Stats[EStat::Stamina] = UKismetMathLibrary::FInterpTo_Constant(
        Stats[EStat::Stamina],
        Stats[EStat::MaxStamina],
        GetWorld()->DeltaTimeSeconds,
        StaminaRegenRate
    );

    OnStaminaPercentUpdateDelegate.Broadcast
    (
        GetStatPercentage(EStat::Stamina, EStat::MaxStamina)
    );

    Stats[EStat::Stamina] = UKismetMathLibrary::FClamp(
        Stats[EStat::Stamina],
        0,
        Stats[EStat::MaxStamina]
    );

    
}

void UStatsComponent::EnableRegen()
{
    bCanRegen = true;
    
}

float UStatsComponent::GetStatPercentage(EStat Current, EStat Max)
{
    return Stats[Current] / Stats[Max];
}

