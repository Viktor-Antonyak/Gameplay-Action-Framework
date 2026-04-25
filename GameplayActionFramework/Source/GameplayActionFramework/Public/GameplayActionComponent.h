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

#include "GameplayActionComponent.generated.h"

#define UE_API GAMEPLAYACTIONFRAMEWORK_API

DECLARE_LOG_CATEGORY_EXTERN(LogGameplayActionComponent, Log, All);

/** Called when Action is fully activated */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionExecuted, FGameplayTag, ActionTag);

/** Called when Action is ended */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActionEnded, FGameplayTag, ActionTag, bool, bWasCanceled);

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
	UGameplayActionComponent(const FObjectInitializer& ObjectInitializer);

	TSharedPtr<FGameplayActionActorInfo> GameplayActionActorInfo;
	TSharedPtr<FGameplayTagContainer> GameplayTagContainer;

	// --- Action Management ---
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	FGameplayActionSpec AddGameplayAction(const TSubclassOf<UGameplayAction> Action, const int32 Level = -1);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void RemoveGameplayAction(const FGameplayActionSpec& ActionSpec);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	bool TryActivateActionBySpec(const FGameplayActionSpec& ActionSpec);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action", meta = (Categories))
	bool TryActivateActionsByTag(UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTagContainer ActionTags);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void PressInputID(int32 InputID);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void ReleaseInputID(int32 InputID);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	TArray<UGameplayAction*> GetActiveActions() const { return ActiveActions; }
	
	void AddActiveAction(UGameplayAction* Action);
	void RemoveActiveAction(UGameplayAction* Action, bool bWasCanceled = false);

	// --- Tag Management ---
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Gameplay Action")
	FGameplayTagContainer GetOwnedGameplayTags() const { return GameplayTagContainer.IsValid() ? *GameplayTagContainer.Get() : FGameplayTagContainer(); }
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void AddOwnedGameplayTag(UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTag Tag);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action", meta = (Categories))
	void AddOwnedGameplayTags(UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTagContainer Tags);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay Action", meta = (Categories))
	void RemoveOwnedGameplayTags(UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTagContainer Tags);

	// --- Attribute Management ---
	UFUNCTION(BlueprintCallable, Category="Gameplay Attribute")
	UGameplayAttributeSet* GetAttributeSetByClass(TSubclassOf<UGameplayAttributeSet> AttributeClass) const;
	
	/** Returns the numeric value of an attribute from the managed sets. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Attribute", meta = (ShowOnlyInnerProperties = "Attribute"))
	float GetAttributeValue(const FGameplayAttribute& Attribute) const;

	/** Sets the numeric value of an attribute, triggering Pre/PostChange callbacks. */
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

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitActorInfo();
	void InitAttributes();

	UPROPERTY()
	TArray<UGameplayAction*> ActiveActions;
	
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
