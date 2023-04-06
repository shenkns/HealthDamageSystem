// Copyright shenkns Health-Damage System Developed With Unreal Engine. All Rights Reserved 2022.

#pragma once

#include "UObject/Object.h"
#include "HealthBase.generated.h"

class UHealthHandlerDataAsset;
class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHealthEndedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHealthChangedEvent, float, NewHealth, float, HealthDelta);

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class HEALTHDAMAGESYSTEM_API UHealthBase : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FHealthEndedEvent OnHealthEnded;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FHealthChangedEvent OnHealthChanged;

protected:

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Health")
	UHealthHandlerDataAsset* HealthHandler;

	UPROPERTY(EditAnywhere, Replicated, Category = "Health")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	bool bStartMaxHealth = true;

	UPROPERTY(EditAnywhere, ReplicatedUsing = "OnRepCurrentHealth", Category = "Health", meta = (EditCondition = "!bStartMaxHealth"))
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	int Priority;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRepIsEnded", Category = "Health")
	bool bIsEnded = false;

private:

	float OldHealth;

public:

	virtual bool IsSupportedForNetworking() const override { return true; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Getters
	UFUNCTION(BlueprintPure, Category = "Health")
	UHealthHandlerDataAsset* GetHealthHandler() const { return HealthHandler; }
	
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	UHealthComponent* GetOwningHealthComponent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	int GetPriority() const { return Priority; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool GetIsHealthEnded() const { return bIsEnded; }

	// Health Actions
	UFUNCTION(BlueprintCallable, Category = "Health", meta = (CompactNodeTitle = "AddHealth"))
	float AddHealth(float Amount);

	void InitHealthObject(UHealthHandlerDataAsset* HandlerDataAsset);

protected:

	UFUNCTION()
	virtual void OnRepCurrentHealth();

	UFUNCTION()
	virtual void OnRepIsEnded();
};
