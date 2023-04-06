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

	if(GetOwner()->HasAuthority())
	{
		for(const TTuple<UHealthHandlerDataAsset*, UHealthBase*>& Pair : HealthObjectsMap)
		{
			Pair.Value->Rename(nullptr, this);
			Pair.Value->InitHealthObject(Pair.Key);

			HealthObjectsArray.Add(Pair.Value);
		}

		LOG(LogHealthDamageSystem, "%d Health Objects Initialized", HealthObjectsArray.Num())
	}
}

bool UHealthComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for(UHealthBase* HealthObject : HealthObjectsArray)
	{
		if(HealthObject)
		{
			bWroteSomething |= Channel->ReplicateSubobject(HealthObject, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UHealthComponent, HealthObjectsArray)
}

UHealthBase* UHealthComponent::GetHealthObjectFromHandler(TSubclassOf<UHealthBase> Class, UHealthHandlerDataAsset* Handler) const
{
	UHealthBase* const* HealthObjectPointer = HealthObjectsArray.FindByPredicate([&](const UHealthBase* HealthObject)
	{
		return HealthObject->GetHealthHandler() == Handler;
	});
	
	return HealthObjectPointer ? *HealthObjectPointer : nullptr;
}

UHealthBase* UHealthComponent::GetPrimaryHealthObject(TSubclassOf<UHealthBase> Class) const
{
	UHealthBase* PrimaryHealthObject = nullptr;
	int PrimaryHealthObjectPriority = -1;

	for(UHealthBase* HealthObject : HealthObjectsArray)
	{
		if(HealthObject->GetPriority() < PrimaryHealthObjectPriority || PrimaryHealthObjectPriority == -1)
		{
			PrimaryHealthObject = HealthObject;
			PrimaryHealthObjectPriority = PrimaryHealthObject->GetPriority();
		}
	}

	return PrimaryHealthObject;
}

TArray<UHealthBase*> UHealthComponent::GetHealthObjectsSortedByPriority() const
{
	TArray<UHealthBase*> HealthObjectsSorted = HealthObjectsArray;

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
		
		if(HealthObject == GetPrimaryHealthObject<UHealthBase>())
		{
			if(HealthObject->GetIsHealthEnded())
			{
				bIsDeath = true;
				OnDeath.Broadcast(Instigator, Causer);

				break;
			}
		}
		
		if(FMath::IsNearlyEqual(0.f, Amount, HealthErrorTolerance))
		{
			break;
		}
	}

	return StartAmount - Amount;
}
