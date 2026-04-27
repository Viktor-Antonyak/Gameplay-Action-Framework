// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "GameplayAction.h"
#include "GameplayActionComponent.h"

DEFINE_LOG_CATEGORY(LogGameplayAction);

UGameplayAction::UGameplayAction()
{
	ActionType = EGameplayActionType::Default;
	bIsActive = false;
	bIsInitialized = false;
	CachedActionLevel = -1;
}

void UGameplayAction::InitializeAction(const TSharedPtr<FGameplayActionActorInfo>& ActorInfo, int32 ActionLevel)
{
	CachedActorInfo = ActorInfo;
	CachedActionLevel = ActionLevel;
	bIsInitialized = true;
}

void UGameplayAction::DeinitializeAction()
{
	CachedActorInfo.Reset();
	CachedActionLevel = -1;
	bIsInitialized = false;
}

bool UGameplayAction::RequestExecuteAction(const TSharedPtr<FGameplayActionActorInfo>& ActorInfo, int32 ActionLevel)
{
	if (!CachedActorInfo.IsValid() || !GetActionComponent())
	{
		return false;
	}
	
	FGameplayTag ThisActionTag = GetActionTag();
	
	// Check blocking tags
	UGameplayAction* const* FoundBlockAction = GetActionComponent()->GetActiveActions().FindByPredicate(
	[ThisActionTag](const UGameplayAction* PredicateAction)
	{
		return PredicateAction && PredicateAction->GetBlockOtherActionsTags().HasTag(ThisActionTag);
	});

	bool bCanceledAllActions = true;
	
	// Handle cancellations
	for (UGameplayAction* ActiveAction : GetActionComponent()->GetActiveActions())
	{
		if (ActiveAction && ActiveAction != this && GetCancelOtherActionsTags().HasTag(ActiveAction->GetActionTag()))
		{
			if (!ActiveAction->RequestCancelAction())
			{
				bCanceledAllActions = false;
				break;
			}
		}
	}
	
	const bool bHasRequiredTags = GetActionComponent()->GetOwnedGameplayTags().HasAll(GameplayActionInfoTags.RequireTags);
	const bool bHasBlockedTags = GetActionComponent()->GetOwnedGameplayTags().HasAny(GameplayActionInfoTags.BlockedByTags);
	
	if (!bIsActive && CanExecuteAction() && !FoundBlockAction && bCanceledAllActions && bHasRequiredTags && !bHasBlockedTags)
	{
		bIsActive = true;
		GetActionComponent()->AddActiveAction(this);
		GetActionComponent()->AddOwnedGameplayTags(GameplayActionInfoTags.GrantTags);
		
		OnExecuteAction();
		return true;
	}
	return false;
}

bool UGameplayAction::RequestTriggerAction(const TSharedPtr<FGameplayActionActorInfo>& ActorInfo, int32 ActionLevel, const FInstancedStruct& Payload)
{
	if (!ActorInfo.IsValid() || !ActorInfo->ActionComponent.IsValid())
	{
		return false;
	}

	InitializeAction(ActorInfo, ActionLevel);
	
	FGameplayTag ThisActionTag = GetActionTag();
	
	// Check blocking tags
	UGameplayAction* const* FoundBlockAction = GetActionComponent()->GetActiveActions().FindByPredicate(
	[ThisActionTag](const UGameplayAction* PredicateAction)
	{
		return PredicateAction && PredicateAction->GetBlockOtherActionsTags().HasTag(ThisActionTag);
	});

	bool bCanceledAllActions = true;
	
	// Handle cancellations
	for (UGameplayAction* ActiveAction : GetActionComponent()->GetActiveActions())
	{
		if (ActiveAction && ActiveAction != this && GetCancelOtherActionsTags().HasTag(ActiveAction->GetActionTag()))
		{
			if (!ActiveAction->RequestCancelAction())
			{
				bCanceledAllActions = false;
				break;
			}
		}
	}
	
	const bool bHasRequiredTags = GetActionComponent()->GetOwnedGameplayTags().HasAll(GameplayActionInfoTags.RequireTags);
	const bool bHasBlockedTags = GetActionComponent()->GetOwnedGameplayTags().HasAny(GameplayActionInfoTags.BlockedByTags);
	
	if (CanExecuteAction() && !FoundBlockAction && bCanceledAllActions && bHasRequiredTags && !bHasBlockedTags)
	{
		bIsActive = true;
		GetActionComponent()->AddTriggeredAction(this);
		GetActionComponent()->AddOwnedGameplayTags(GameplayActionInfoTags.GrantTags);
		
		OnActionTriggered(Payload);
		return true;
	}
	return false;
}

void UGameplayAction::RequestEndAction()
{
	if (bIsActive)
	{
		EndAction();
	}
}

bool UGameplayAction::RequestCancelAction()
{
	if (bIsActive && bCanBeCanceled)
	{
		CancelAction();
		return true;
	}
	return false;
}

UWorld* UGameplayAction::GetWorld() const
{
	if (CachedActorInfo.IsValid() && CachedActorInfo->OwnerActor.IsValid())
	{
		return CachedActorInfo->OwnerActor->GetWorld();
	}
	return nullptr;
}

// Start of IGameplayTaskOwnerInterface
UGameplayTasksComponent* UGameplayAction::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return GetActionComponent();
}

AActor* UGameplayAction::GetGameplayTaskOwner(const UGameplayTask* Task) const
{
	return GetOwnerActor();
}

AActor* UGameplayAction::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	return GetOwnerActor();
}

void UGameplayAction::OnGameplayTaskInitialized(UGameplayTask& Task)
{
	IGameplayTaskOwnerInterface::OnGameplayTaskInitialized(Task);
}

void UGameplayAction::OnGameplayTaskActivated(UGameplayTask& Task)
{
	IGameplayTaskOwnerInterface::OnGameplayTaskActivated(Task);
}

void UGameplayAction::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	IGameplayTaskOwnerInterface::OnGameplayTaskDeactivated(Task);
}
// End of IGameplayTaskOwnerInterface

void UGameplayAction::EndAction()
{
	if (!GetActionComponent())
	{
		return;
	}
	
	OnEndAction(false);
		
	bIsActive = false;
	bCanBeCanceled = true;
	
	if (ActionType == EGameplayActionType::Default)
	{
		GetActionComponent()->RemoveActiveAction(this, false);
	}
	else
	{
		GetActionComponent()->RemoveTriggeredAction(this);
	}
	
	GetActionComponent()->RemoveOwnedGameplayTags(GameplayActionInfoTags.GrantTags);
}

void UGameplayAction::CancelAction()
{
	if (!GetActionComponent())
	{
		return;
	}
	
	OnEndAction(true);
	
	bIsActive = false;
	bCanBeCanceled = true;
	
	if (ActionType == EGameplayActionType::Default)
	{
		GetActionComponent()->RemoveActiveAction(this, true);
	}
	else
	{
		GetActionComponent()->RemoveTriggeredAction(this);
	}
	
	GetActionComponent()->RemoveOwnedGameplayTags(GameplayActionInfoTags.GrantTags);
}

TArray<USkeletalMeshComponent*> UGameplayAction::GetSkeletalMeshComponents() const
{
	TArray<USkeletalMeshComponent*> RawSkeletalMeshComponents;

	if (CachedActorInfo.IsValid())
	{
		for (const TWeakObjectPtr<USkeletalMeshComponent>& SkeletalMeshComponent : CachedActorInfo->SkeletalMeshComponents)
		{
			if (SkeletalMeshComponent.IsValid())
			{
				RawSkeletalMeshComponents.Add(SkeletalMeshComponent.Get());
			}
		}
	}
	return RawSkeletalMeshComponents;
}

void UGameplayAction::OnExecuteAction_Implementation() {}
void UGameplayAction::OnActionTriggered_Implementation(const FInstancedStruct& Payload) {}
void UGameplayAction::OnEndAction_Implementation(bool bWasCanceled) {}
bool UGameplayAction::CanExecuteAction_Implementation() const { return true; }
