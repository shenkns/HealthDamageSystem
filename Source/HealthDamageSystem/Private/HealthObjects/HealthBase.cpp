// Copyright shenkns Health-Damage System Developed With Unreal Engine. All Rights Reserved 2023.

#include "HealthObjects/HealthBase.h"

#include "Log.h"
#include "Module/HealthDamageSystemModule.h"
#include "Components/HealthComponent.h"
#include "Log/Details/LocalLogCategory.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_LOCAL(LogHealthDamageSystem);

void UHealthBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthBase, MaxHealth);
	DOREPLIFETIME(UHealthBase, CurrentHealth);

	DOREPLIFETIME(UHealthBase, bIsEnded);

	DOREPLIFETIME(UHealthBase, HealthHandler);
	
	if(const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}
}

bool UHealthBase::Reset(bool bForceMaxHealth)
{
	if(bStartMaxHealth || bForceMaxHealth)
	{
		CurrentHealth = MaxHealth;
	}

	bIsEnded = CurrentHealth <= 0.f;

	return !bIsEnded;
}

UHealthComponent* UHealthBase::GetOwningHealthComponent() const
{
	return GetTypedOuter<UHealthComponent>();
}

float UHealthBase::AddHealth_Implementation(float Amount)
{
	const float PreviousHealth = CurrentHealth;
	
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, CurrentHealth - PreviousHealth);

	if(PreviousHealth + Amount <= 0.f)
	{
		bIsEnded = true;
	}

	return CurrentHealth - PreviousHealth;
}

void UHealthBase::InitHealthObject(UHealthHandlerDataAsset* HealthHandlerDataAsset)
{
	HealthHandler = HealthHandlerDataAsset;
	
	if(bStartMaxHealth)
	{
		CurrentHealth = MaxHealth;
	}

	LOG(Display, "{} Health Object Intializaed", *GetName());
}

void UHealthBase::OnRepCurrentHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth, CurrentHealth - OldHealth);

	LOG(Display, "{} Health Changed On {} From {} To {}", *GetName(), CurrentHealth - OldHealth, OldHealth, CurrentHealth);

	OldHealth = CurrentHealth;
}

void UHealthBase::OnRepIsEnded()
{
	if(bIsEnded)
	{
		OnHealthEnded.Broadcast();

		LOG(Display, "{} Health Ended", *GetName());
	}
}
