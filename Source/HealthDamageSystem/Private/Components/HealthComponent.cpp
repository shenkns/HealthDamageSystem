// Copyright shenkns Health-Damage System Developed With Unreal Engine. All Rights Reserved 2022.

#include "Components/HealthComponent.h"

#include "Module/HealthDamageSystemModule.h"
#include "LogSystem.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	for(const TTuple<UHealthHandlerDataAsset*, UHealthBase*>& Pair : HealthObjectsMap)
	{
		Pair.Value->Rename(nullptr, this);
		Pair.Value->InitHealthObject();

		HealthObjectsArray.Add(Pair.Value);
	}

	LOG(LogHealthDamageSystem, "%d Health Objects Initialized", HealthObjectsMap.Num())
}

bool UHealthComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	for(UHealthBase* HealthObject : HealthObjectsArray)
	{
		
	}

	return Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UHealthComponent, HealthObjectsArray)
}

UHealthBase* UHealthComponent::GetHealthObjectFromHandler(TSubclassOf<UHealthBase> Class, UHealthHandlerDataAsset* Handler) const
{
	UHealthBase* const* HealthObjectPointer = HealthObjectsMap.Find(Handler);
	return HealthObjectPointer ? *HealthObjectPointer : nullptr;
}

UHealthBase* UHealthComponent::GetPrimaryHealthObject(TSubclassOf<UHealthBase> Class) const
{
	UHealthBase* PrimaryHealthObject = nullptr;
	int PrimaryHealthObjectPriority = -1;

	for(const TTuple<UHealthHandlerDataAsset*, UHealthBase*> Pair : HealthObjectsMap)
	{
		if(Pair.Value->GetPriority() < PrimaryHealthObjectPriority || PrimaryHealthObjectPriority == -1)
		{
			PrimaryHealthObject = Pair.Value;
			PrimaryHealthObjectPriority = PrimaryHealthObject->GetPriority();
		}
	}

	return PrimaryHealthObject;
}

TArray<UHealthBase*> UHealthComponent::GetHealthObjectsSortedByPriority() const
{
	TArray<UHealthBase*> HealthObjectsSorted;
	HealthObjectsMap.GenerateValueArray(HealthObjectsSorted);

	HealthObjectsSorted.Sort([&](const UHealthBase &HealthObjectA, const UHealthBase &HealthObjectB)
	{
		return HealthObjectA.GetPriority() > HealthObjectB.GetPriority();
	});

	return HealthObjectsSorted;
}

float UHealthComponent::Damage(AActor* Target, float Amount, AController* Instigator, AActor* Causer)
{
	if(!Target) return 0.f;
	
	if(UHealthComponent* HealthComponent = Target->FindComponentByClass<UHealthComponent>())
	{
		return -HealthComponent->AddHealth(-Amount, Instigator, Causer);
	}

	return 0.f;
}

float UHealthComponent::Recover(AActor* Target, float Amount, AController* Instigator, AActor* Causer)
{
	if(!Target) return 0.f;
	
	if(UHealthComponent* HealthComponent = Target->FindComponentByClass<UHealthComponent>())
	{
		return HealthComponent->AddHealth(Amount, Instigator, Causer);
	}

	return 0.f;
}

float UHealthComponent::AddHealth(float Amount, AController* Instigator, AActor* Causer)
{
	if(bIsDeath) return 0.f;

	const float StartAmount = Amount;
	
	for(UHealthBase* HealthObject : GetHealthObjectsSortedByPriority())
	{
		Amount -= HealthObject->AddHealth(Amount);
		
		if(FMath::IsNearlyEqual(0.f, Amount, HealthErrorTolerance))
		{
			if(HealthObject == GetPrimaryHealthObject<UHealthBase>())
			{
				if(HealthObject->GetIsHealthEnded())
				{
					bIsDeath = true;
					OnDeath.Broadcast(Instigator, Causer);
				}
			}

			return StartAmount - Amount;
		}
	}

	if(StartAmount < 0)
	{
		bIsDeath = true;
		OnDeath.Broadcast(Instigator, Causer);
	}

	return StartAmount - Amount;
}
