// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayActionActorInfo.h"
#include "GameplayActionSpec.h"
#include "GameplayAttributeSet.h"
#include "Templates/SubclassOf.h"
#include "GameplayTasksComponent.h"
#include "InstancedStruct.h"

#include "GameplayActionComponent.generated.h"

#define UE_API GAMEPLAYACTIONFRAMEWORK_API

DECLARE_LOG_CATEGORY_EXTERN(LogGameplayActionComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionExecuted, const FGameplayTag&, ActionTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActionEnded, const FGameplayTag&, ActionTag, bool, bWasCanceled);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriggeredActionActivated, TSubclassOf<UGameplayAction>, ActionClass);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriggeredActionEnded, TSubclassOf<UGameplayAction>, ActionClass);

class UGameplayAction;

/** Data for initializing specific attributes on the component */
USTRUCT(BlueprintType)
struct FAttributeInitializationData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay Attribute")
	FGameplayAttribute Attribute;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay Attribute")
	float InitialValue = 0.0f;
};

/** Component that manages Gameplay Actions and Attributes. */
UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent), MinimalAPI)
class UGameplayActionComponent : public UGameplayTasksComponent
{
	GENERATED_BODY()

public:
	explicit UGameplayActionComponent(const FObjectInitializer& ObjectInitializer);

	TSharedPtr<FGameplayActionActorInfo> GameplayActionActorInfo;

	// --- Action Management ---
	
	/** Adds Gameplay Action to available actions */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	FGameplayActionSpec AddGameplayAction(const TSubclassOf<UGameplayAction> Action, const int32 Level = -1);
	
	/** Removes Gameplay Action from available actions */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void RemoveGameplayAction(const FGameplayActionSpec& ActionSpec);
	
	/** Activate Action by Action Spec */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	bool TryActivateActionBySpec(const FGameplayActionSpec& ActionSpec);
	
	/** Activate Action bu Gameplay Tag */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	bool TryActivateActionsByTag(UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTagContainer ActionTags);
	
	/** Activate Action by pressing Input ID defined in the action. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void PressInputID(int32 InputID);
	
	/** End Action by releasing Input ID defined in the action. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void ReleaseInputID(int32 InputID);
	
	/** Spawns and triggers a new action of the specified class with payload. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void TriggerActionByClass(TSubclassOf<UGameplayAction> ActionClass, const FInstancedStruct& Payload, int32 ActionLevel = -1);
	
	/** Return array of active actions. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	TArray<UGameplayAction*> GetActiveActions() const { return ActiveActions; }
	
	// Default Actions
	void AddActiveAction(UGameplayAction* Action);
	void RemoveActiveAction(UGameplayAction* Action, bool bWasCanceled = false);

	// Triggered Actions
	void AddTriggeredAction(UGameplayAction* Action);
	void RemoveTriggeredAction(UGameplayAction* Action);
	
	// --- Tag Management ---
	
	/** Return owned Gameplay Tags from component */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Gameplay Action")
	FGameplayTagContainer GetOwnedGameplayTags() const { return GameplayTagContainer.IsValid() ? *GameplayTagContainer.Get() : FGameplayTagContainer(); }
	
	/** Add Gameplay Tags to component */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void AddOwnedGameplayTags(UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTagContainer Tags);
	
	/** Remove Gameplay Tags from component */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void RemoveOwnedGameplayTags(UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTagContainer Tags);

	// --- Attribute Management ---
	
	/** Return specific owned Attribute Set from component */
	UFUNCTION(BlueprintCallable, Category="Gameplay Attribute")
	UGameplayAttributeSet* GetAttributeSetByClass(TSubclassOf<UGameplayAttributeSet> AttributeClass) const;
	
	/** Returns the float value of an attribute from the attribute sets. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Attribute")
	float GetAttributeValue(const FGameplayAttribute& Attribute) const;

	/** Sets the float value of an attribute, triggering Pre/PostChange callbacks. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Attribute")
	void SetAttributeValue(const FGameplayAttribute& Attribute, float NewValue);

	/** Adds an attribute set to this component. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Attribute")
	UGameplayAttributeSet* AddAttributeSet(TSubclassOf<UGameplayAttributeSet> AttributeSetClass);

	// Delegates
	UPROPERTY(BlueprintAssignable, Category="Gameplay Action")
	FActionExecuted OnActionExecuted;
	
	UPROPERTY(BlueprintAssignable, Category="Gameplay Action")
	FActionEnded OnActionEnded;
	
	UPROPERTY(BlueprintAssignable, Category="Gameplay Action")
	FTriggeredActionActivated OnTriggeredActionActivated;
	
	UPROPERTY(BlueprintAssignable, Category="Gameplay Action")
	FTriggeredActionEnded OnTriggeredActionEnded;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitActorInfo();
	void InitAttributes();

	TSharedPtr<FGameplayTagContainer> GameplayTagContainer;
	
	UPROPERTY()
	TArray<UGameplayAction*> ActiveActions;
	
	UPROPERTY()
	TArray<UGameplayAction*> ActiveTriggeredActions;
	
	UPROPERTY()
	TArray<FGameplayActionSpec> AddedActions;
	
	/** Attribute sets to be created on BeginPlay */
	UPROPERTY(EditAnywhere, Category="Gameplay Attributes")
	TArray<TSubclassOf<UGameplayAttributeSet>> DefaultAttributes;
	
	/** Specific initial values for attributes (applied after set creation) */
	UPROPERTY(EditAnywhere, Category="Gameplay Attributes")
	TArray<FAttributeInitializationData> InitialAttributeValues;
	
	/** All active attribute sets managed by this component */
	UPROPERTY(BlueprintReadOnly, Category="Gameplay Attributes")
	TArray<UGameplayAttributeSet*> AttributeSets;
};

#undef UE_API
