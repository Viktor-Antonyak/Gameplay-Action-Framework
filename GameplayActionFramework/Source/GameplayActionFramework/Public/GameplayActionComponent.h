// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved. Gameplay Action Framework.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayActionActorInfo.h"
#include "GameplayActionSpec.h"
#include "Templates/SharedPointer.h"
#include "Templates/SubclassOf.h"
#include "GameplayTasksComponent.h"

#include "GameplayActionComponent.generated.h"

#define UE_API GAMEPLAYACTIONFRAMEWORK_API

DECLARE_LOG_CATEGORY_EXTERN(LogGameplayActionComponent, Log, All);

/** Called when Action is fully activated */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionExecuted, FGameplayTag, ActionTag);

/** Called when Action is ended by another Action */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActionEnded, FGameplayTag, ActionTag, bool, bWasCanceled);

class UGameplayAction;

/** Component that manages Gameplay Actions. This component is responsible for adding, removing, and activating actions, as well as managing the actor's gameplay tags. */
UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent), MinimalAPI)
class UGameplayActionComponent : public UGameplayTasksComponent
{
	GENERATED_BODY()

public:
	
	UGameplayActionComponent(const FObjectInitializer& ObjectInitializer);
	
	TSharedPtr<FGameplayActionActorInfo> GameplayActionActorInfo;
	
	TSharedPtr<FGameplayTagContainer> GameplayTagContainer;
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	FGameplayActionSpec AddGameplayAction(const TSubclassOf<UGameplayAction> Action, const int32 Level = -1);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void RemoveGameplayAction(const FGameplayActionSpec& ActionSpec);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	bool TryActivateActionBySpec(const FGameplayActionSpec& ActionSpec);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	bool TryActivateActionsByTag(const FGameplayTagContainer& ActionTag);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void PressInputID(int32 InputID);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void ReleaseInputID(int32 InputID);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	TArray<UGameplayAction*> GetActiveActions() const { return ActiveActions; }
	
	void AddActiveAction(UGameplayAction* Action);
	
	void RemoveActiveAction(UGameplayAction* Action, bool bWasCanceled = false);
	
	UFUNCTION(BlueprintGetter, Category="Gameplay Action")
	FGameplayTagContainer GetOwnedGameplayTags() const { return GameplayTagContainer.IsValid() ? *GameplayTagContainer.Get() : FGameplayTagContainer(); }
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void AddOwnedGameplayTag(const FGameplayTag& Tag);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void AddOwnedGameplayTags(const FGameplayTagContainer& Tags);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void RemoveOwnedGameplayTags(const FGameplayTagContainer& Tags);
	
	// Delegates
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Gameplay Action")
	FActionExecuted OnActionExecuted;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Gameplay Action")
	FActionEnded OnActionEnded;	

protected:
	
	void InitActorInfo();
	
	UPROPERTY()
	TArray<UGameplayAction*> ActiveActions;
	
	UPROPERTY()
	TArray<FGameplayActionSpec> AddedActions;

public:
	
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
};

#undef UE_API
