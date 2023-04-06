// Copyright shenkns Health-Damage System Developed With Unreal Engine. All Rights Reserved 2022.

#pragma once

#include "Components/ActorComponent.h"

#include "HealthObjects/HealthBase.h"

#include "HealthComponent.generated.h"

class UHealthHandlerDataAsset;
class UHealthBase;
class AActor;
class AController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDeathEvent, AController*, Instigator, AActor*, Causer);

UCLASS(ClassGroup=(Health), meta=(BlueprintSpawnableComponent))
class HEALTHDAMAGESYSTEM_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UHealthComponent();

public:

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FDeathEvent OnDeath;
	
protected:

	UPROPERTY(EditAnywhere, Instanced, Category = "Health")
	TMap<UHealthHandlerDataAsset*, UHealthBase*> HealthObjectsMap;

	UPROPERTY(Replicated)
	TArray<UHealthBase*> HealthObjectsArray;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float HealthErrorTolerance = 0.001;

	UPROPERTY(Replicated)
	bool bIsDeath = false;

public:

	virtual void BeginPlay() override;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Getters
	UFUNCTION(BlueprintPure, Category = "Health", meta = (CompactNodeTitle = "HealthObject", DeterminesOutputType = "Class"))
	UHealthBase* GetHealthObjectFromHandler(TSubclassOf<UHealthBase> Class, UHealthHandlerDataAsset* Handler) const;

	UFUNCTION(BlueprintPure, Category = "Health", meta = (CompactNodeTitle = "PrimaryHealthObject", DeterminesOutputType = "Class"))
	UHealthBase* GetPrimaryHealthObject(TSubclassOf<UHealthBase> Class) const;

	UFUNCTION(BlueprintPure, Category = "Health", meta = (CompactNodeTitle = "HealthObjectsByPriority"))
	TArray<UHealthBase*> GetHealthObjectsSortedByPriority() const;

	UFUNCTION(BlueprintPure, Category = "Health", meta = (CompactNodeTitle = "Death"))
	bool GetIsDeath() const { return bIsDeath; };

	template<typename T>
	T* GetHealthObjectFromHandler(UHealthHandlerDataAsset* Handler) const;

	template<typename T>
	T* GetPrimaryHealthObject() const;

	// Static Health Actions
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "HealthDamageSystem", meta = (CompactNodeTitle = "Damage"))
	static float Damage(AActor* Target, float Amount, AController* Instigator, AActor* Causer);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "HealthDamageSystem", meta = (CompactNodeTitle = "Recover"))
	static float Recover(AActor* Target, float Amount, AController* Instigator, AActor* Causer);

protected:

	float AddHealth(float Amount, AController* Instigator, AActor* Causer);
};

template <typename T>
T* UHealthComponent::GetHealthObjectFromHandler(UHealthHandlerDataAsset* Handler) const
{
	UHealthBase* const* HealthObject = HealthObjectsMap.Find(Handler);

	return HealthObject ? static_cast<T*>(*HealthObject) : nullptr;
}

template <typename T>
T* UHealthComponent::GetPrimaryHealthObject() const
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

	return PrimaryHealthObject ? static_cast<T*>(PrimaryHealthObject) : PrimaryHealthObject;
}
