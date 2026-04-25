// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.


#include "GameplayAction.h"

#include "GameplayActionComponent.h"

DEFINE_LOG_CATEGORY(LogGameplayAction);

UGameplayAction::UGameplayAction()
{
	
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
	
	UGameplayAction* const* FoundBlockAction = GetActionComponent()->GetActiveActions().FindByPredicate(
	[ThisActionTag](const UGameplayAction* PredicateAction)
	{
		return PredicateAction && PredicateAction->GetBlockOtherActionsTags().HasTag(ThisActionTag);
	});

	bool bCanceledAllActions = true;
	
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
	if (bIsInitialized)
	{
		return GetOuter()->GetWorld();
	}
	return nullptr;
}

UGameplayTasksComponent* UGameplayAction::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return GetActorInfo().ActionComponent.IsValid() ? GetActorInfo().ActionComponent.Get() : nullptr;
}

AActor* UGameplayAction::GetGameplayTaskOwner(const UGameplayTask* Task) const
{
	return GetActorInfo().OwnerActor.IsValid() ? GetActorInfo().OwnerActor.Get() : nullptr;
}

AActor* UGameplayAction::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	return GetActorInfo().OwnerActor.IsValid() ? GetActorInfo().OwnerActor.Get() : nullptr;
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

void UGameplayAction::EndAction()
{
	if (!GetActionComponent())
	{
		return;
	}
	
	OnEndAction(false);
		
	bIsActive = false;
		
	bCanBeCanceled = true;
	
	GetActionComponent()->RemoveActiveAction(this, false);
	
	GetActionComponent()->RemoveOwnedGameplayTags(GameplayActionInfoTags.GrantTags);
}

void UGameplayAction::CancelAction()
{
	if (!GetActionComponent())
	{
		return;
	}
	
	OnEndAction(true);
	
	bCanBeCanceled = true;
	
	bIsActive = false;
	
	GetActionComponent()->RemoveActiveAction(this, true);
	
	GetActionComponent()->RemoveOwnedGameplayTags(GameplayActionInfoTags.GrantTags);
}

TArray<USkeletalMeshComponent*> UGameplayAction::GetSkeletalMeshComponents() const
{
	TArray<USkeletalMeshComponent*> RawSkeletalMeshComponents;

	if (!CachedActorInfo.IsValid())
	{
		return RawSkeletalMeshComponents;
	}
	
	for (const TWeakObjectPtr<USkeletalMeshComponent>& SkeletalMeshComponent : CachedActorInfo->SkeletalMeshComponents)
	{
		if (SkeletalMeshComponent.IsValid())
		{
			RawSkeletalMeshComponents.Add(SkeletalMeshComponent.Get());
		}
	}
	
	return RawSkeletalMeshComponents;
}

void UGameplayAction::OnExecuteAction_Implementation()
{
	
}

void UGameplayAction::OnEndAction_Implementation(bool bWasCanceled)
{
	
}

bool UGameplayAction::CanExecuteAction_Implementation() const
{
	return true;
}
