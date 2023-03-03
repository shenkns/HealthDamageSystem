// Copyright shenkns Health-Damage System Developed With Unreal Engine. All Rights Reserved 2022.

#include "HealthObjects/HealthBase.h"

#include "Module/HealthDamageSystemModule.h"
#include "Components/HealthComponent.h"
#include "LogSystem.h"
#include "Net/UnrealNetwork.h"

void UHealthBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthBase, MaxHealth);
	DOREPLIFETIME(UHealthBase, CurrentHealth);
}

UHealthComponent* UHealthBase::GetOwningHealthComponent() const
{
	return GetTypedOuter<UHealthComponent>();
}

float UHealthBase::AddHealth(float Amount)
{
	const float OldHealth = CurrentHealth;

	CurrentHealth = FMath::Clamp(OldHealth + Amount, 0.f, MaxHealth);

	const float HealthDelta = CurrentHealth - OldHealth;

	OnHealthChanged.Broadcast(CurrentHealth, HealthDelta);

	LOG(LogHealthDamageSystem, "%s Health Changed On %f From %f To %f", *GetName(), HealthDelta, OldHealth, CurrentHealth)

	if(OldHealth + Amount <= 0.f)
	{
		bIsEnded = true;
		OnHealthEnded.Broadcast();

		LOG(LogHealthDamageSystem, "%s Health Ended", *GetName())
	}

	return HealthDelta;
}

void UHealthBase::InitHealthObject()
{
	if(bStartMaxHealth)
	{
		CurrentHealth = MaxHealth;
	}

	LOG(LogHealthDamageSystem, "%s Health Object Intializaed", *GetName())
}
