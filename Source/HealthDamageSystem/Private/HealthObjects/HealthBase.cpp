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

	DOREPLIFETIME(UHealthBase, HealthHandler)
	
	if(const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}
}

UHealthComponent* UHealthBase::GetOwningHealthComponent() const
{
	return GetTypedOuter<UHealthComponent>();
}

float UHealthBase::AddHealth(float Amount)
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

	LOG(LogHealthDamageSystem, "%s Health Object Intializaed", *GetName())
}

void UHealthBase::OnRepCurrentHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth, CurrentHealth - OldHealth);

	LOG(LogHealthDamageSystem, "%s Health Changed On %f From %f To %f", *GetName(), CurrentHealth - OldHealth, OldHealth, CurrentHealth)

	OldHealth = CurrentHealth;
}

void UHealthBase::OnRepIsEnded()
{
	if(bIsEnded)
	{
		OnHealthEnded.Broadcast();

		LOG(LogHealthDamageSystem, "%s Health Ended", *GetName())
	}
}
