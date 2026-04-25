// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameplayActionActorInfo.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Templates/SharedPointer.h"
#include "Templates/SubclassOf.h"
#include "GameplayTaskOwnerInterface.h"

#include "GameplayAction.generated.h"

#define UE_API GAMEPLAYACTIONFRAMEWORK_API

DECLARE_LOG_CATEGORY_EXTERN(LogGameplayAction, Log, All);

/** 
 * Struct that defines the tags associated with a gameplay action. These tags are used by the Gameplay Action to determine how actions interact with each other. 
 * For example, if an active action has a Block Other Actions Tag that matches the Gameplay Action Tag of another action, that action cannot be executed.
 */
USTRUCT(MinimalAPI, BlueprintType)
struct FGameplayActionInfoTags
{
	GENERATED_BODY()
	
	/** Gameplay Tag associated with this action. This tag is used to identify the action and to determine how it interacts with other actions. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Gameplay Action Tags")
	FGameplayTag GameplayActionTag;
	
	/** If an active action has a Block Other Actions Tag that matches the Gameplay Action Tag of this action, this action cannot be executed. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Gameplay Action Tags")
	FGameplayTagContainer BlockOtherActions;
	
	/** If an active action has a Cancel Other Actions Tag that matches the Gameplay Action Tag of this action, that action will be canceled when this action is executed. 
	 * 
	 * NOTE: If cancel action has CanBeCanceled set to false, it will not be canceled even if it has matching Cancel Other Actions Tag.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Gameplay Action Tags")
	FGameplayTagContainer CancelOtherActions;
	
	/** Action will only be executed if the actor has all the tags in this container. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Gameplay Action Tags")
	FGameplayTagContainer RequireTags;
	
	/** Action will not be executed if the actor has any of the tags in this container. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Gameplay Action Tags")
	FGameplayTagContainer BlockedByTags;
	
	/** Tags that will be added to the actor when the action is executed. These tags will be removed when the action is ended or canceled. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Gameplay Action Tags")
	FGameplayTagContainer GrantTags;
};

/**
 * Base class for gameplay actions. Gameplay actions are used to define actions that can be performed by actors in the game. 
 * They can be used to define abilities, skills, or any other type of action that an actor can perform.
 */
UCLASS(MinimalAPI, Blueprintable, BlueprintType, Abstract)
class UGameplayAction : public UObject, public IGameplayTaskOwnerInterface
{
	GENERATED_BODY()
	
public:
	
	UGameplayAction();
	
	void InitializeAction(const TSharedPtr<FGameplayActionActorInfo>& ActorInfo, int32 ActionLevel);
	
	void DeinitializeAction();
	
	/** Input ID associated with the action. This can be used to trigger the action using input bindings. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Gameplay Action")
	int32 InputID;
	
	/** Whether the action is currently active. This is set to true when the action is executed and set to false when the action is ended or canceled. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	bool IsActive() const { return bIsActive; }
	
	/** Whether the action can be canceled. If true, the action can be canceled by other actions that have the appropriate cancel tags. If false, the action cannot be canceled once it is executed. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	bool CanBeCanceled() const { return bCanBeCanceled; }
	
	/** Sets whether the action can be canceled. If true, the action can be canceled by other actions that have the appropriate cancel tags. If false, the action cannot be canceled once it is executed. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void SetCanBeCanceled(bool CanBeCanceled) { bCanBeCanceled = CanBeCanceled; }
	
	/** Requests to execute the action. This will check if the action can be executed by calling CanExecuteAction() and if it can, it will call ExecuteAction(). 
	 * 
	 * Note: This must be called in Gameplay Action Component's Action execution functions (e.g., TryActivateActionBySpec, TryActivateActionsByTag, PressInputID) 
	 * to ensure that the action is executed properly and that the Gameplay Action Component can manage the action's state and interactions with other actions.
	 */
	bool RequestExecuteAction(const TSharedPtr<FGameplayActionActorInfo>& ActorInfo, int32 ActionLevel = -1);
	
	/** Requests to end the action. This will call EndAction() and set bIsActive to false. 
	 * 
	 * Note: This must be called in Gameplay Action Component's Action ending functions (e.g., ReleaseInputID)
	 * to ensure that the action is ended properly and that the Gameplay Action Component can manage the action's state and interactions with other actions.
	 */
	void RequestEndAction();
	
	/** Requests to cancel the action. This will call CancelAction() and set bIsActive to false. 
	 * 
	 * Note: This must be called in Gameplay Action Component's Action canceling functions (e.g., when another action with appropriate cancel tags is executed)
	 * to ensure that the action is canceled properly and that the Gameplay Action Component can manage the action's state and interactions with other actions.
	 */
	bool RequestCancelAction();
	
	/** Returns the Gameplay Tag associated with this action in GameplayActionInfoTags. */
	FGameplayTag GetActionTag() const { return GameplayActionInfoTags.GameplayActionTag; }
	
	/** Returns the Block Other Actions Tags associated with this action in GameplayActionInfoTags. */
	FGameplayTagContainer GetBlockOtherActionsTags() const { return GameplayActionInfoTags.BlockOtherActions; }
	
	/** Returns the Cancel Other Actions Tags associated with this action in GameplayActionInfoTags. */
	FGameplayTagContainer GetCancelOtherActionsTags() const { return GameplayActionInfoTags.CancelOtherActions; }
	
protected:
	
	// Override GetWorld() to return the world of the owner actor. This is necessary for the action to be able to spawn actors, play effects, etc. in the correct world context.
	virtual UWorld* GetWorld() const override;
	
	// Start of IGameplayTaskOwnerInterface implementation
	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;
	
	virtual AActor* GetGameplayTaskOwner(const UGameplayTask* Task) const override;
	
	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;
	
	virtual void OnGameplayTaskInitialized(UGameplayTask& Task) override;
	
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	// End of IGameplayTaskOwnerInterface implementation
	
	/** Call this when you want to end the action. This will call OnEndAction(). */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void EndAction();
	
	/** Call this when you want to cancel the action. This will call OnEndAction() with bWasCanceled = true. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	void CancelAction();
	
	/** Called when the action is executed. This is where you should put the logic for what happens when the action is performed. */
	UFUNCTION(BlueprintNativeEvent, Category="Gameplay Action")
	void OnExecuteAction();
	
	/** Called when the action is ended. This is where you should put the logic for what happens when the action is ended, such as resetting variables.
	 * 
	 * NOTE: The bWasCanceled parameter indicates whether the action was canceled. 
	 * If true, the action was canceled. If false, the action ended normally.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Gameplay Action")
	void OnEndAction(bool bWasCanceled = false);
	
	/** Called to check if the action can be executed. This is where you should put the logic for checking if the action can be performed, such as checking if the actor has enough resources or if the action is on cooldown. */
	UFUNCTION(BlueprintNativeEvent, Category="Gameplay Action")
	bool CanExecuteAction() const;
	
	/** Returns Actor Info struct with cached references to the actor's components. This is useful for actions that need to access the actor's components frequently. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action | Action Info")
	FGameplayActionActorInfo GetActorInfo() const { return CachedActorInfo.IsValid() ? *CachedActorInfo.Get() : FGameplayActionActorInfo(); }
	
	/** Returns the level of the action. This is useful for actions that have different levels of power or effectiveness. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action | Action Info")
	int32 GetActionLevel() const { return CachedActionLevel; }
	
	/** Returns the owner actor of the action. This is useful for actions that need to access the actor that owns the action. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action | Action Info")
	AActor* GetOwnerActor() const { return CachedActorInfo ? CachedActorInfo->OwnerActor.Get() : nullptr; }
	
	/** Returns the skeletal mesh components of the owner actor. This is useful for actions that need to access the actor's skeletal mesh components. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action | Action Info")
	TArray<USkeletalMeshComponent*> GetSkeletalMeshComponents() const;
	
	/** Returns the movement component of the owner actor. This is useful for actions that need to access the actor's movement component. 
	 * 
	 * Note: This function returns base UMovementComponent, so if you need to access specific movement component functions, 
	 * you will need to cast it to the appropriate subclass (e.g., UCharacterMovementComponent).
	 */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action | Action Info")
	UMovementComponent* GetMovementComponent() const { return CachedActorInfo ? CachedActorInfo->MovementComponent.Get() : nullptr; }
	
	/** Returns the Gameplay Action Component of the owner actor. This is useful for actions that need to access the actor's Gameplay Action Component. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action | Action Info")
	UGameplayActionComponent* GetActionComponent() const { return CachedActorInfo ? CachedActorInfo->ActionComponent.Get() : nullptr; }
	
	/** Gameplay Action Info Tags are used to define the tags associated with the action, such as the action tag, block tags, cancel tags, etc. These tags are used by the Gameplay Action Component to determine how actions interact with each other. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Gameplay Action", meta=(ShowOnlyInnerProperties))
	FGameplayActionInfoTags GameplayActionInfoTags;
	
private:
	
	/** Whether the action is currently active. This is set to true when the action is executed and set to false when the action is ended or canceled. */
	UPROPERTY()
	bool bIsActive;
	
	/** Is action initialized */
	UPROPERTY()
	bool bIsInitialized;
	
	/** Actor Info struct. */
	TSharedPtr<FGameplayActionActorInfo> CachedActorInfo;
	
	/** Level of this action. */
	UPROPERTY()
	int32 CachedActionLevel;
	
	/** If true, the action can be canceled by other actions that have the appropriate cancel tags. If false, the action cannot be canceled once it is executed. */
	UPROPERTY()
	bool bCanBeCanceled = true;
};

#undef UE_API
