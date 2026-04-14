// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved. Gameplay Action Framework.


#include "GameplayActionComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayActionComponent)

DEFINE_LOG_CATEGORY(LogGameplayActionComponent);

UGameplayActionComponent::UGameplayActionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

FGameplayActionSpec UGameplayActionComponent::AddGameplayAction(const TSubclassOf<UGameplayAction> Action, const int32 Level)
{
	if (!IsValid(Action) || !GameplayActionActorInfo.IsValid())
	{
		UE_LOG(LogGameplayActionComponent, Warning, TEXT("AddGameplayAction: Invalid Action Class"));
		
		return FGameplayActionSpec();
	}
	
	UGameplayAction* NewAction = NewObject<UGameplayAction>(this, Action);
	
	FGameplayActionSpec NewActionSpec;
	NewActionSpec.Action = NewAction;
	NewActionSpec.Level = Level;
	NewActionSpec.InputID = NewAction->InputID;
	
	AddedActions.Add(NewActionSpec);
	
	NewAction->InitializeAction(GameplayActionActorInfo, Level);
	
	return NewActionSpec;
}

void UGameplayActionComponent::RemoveGameplayAction(const FGameplayActionSpec& ActionSpec)
{
	AddedActions.Remove(ActionSpec);

	if (ActiveActions.Contains(ActionSpec.Action))
	{
		ActionSpec.Action->RequestEndAction();
	}
	
	ActionSpec.Action->DeinitializeAction();
	
	ActionSpec.Action->MarkAsGarbage();
}

bool UGameplayActionComponent::TryActivateActionBySpec(const FGameplayActionSpec& ActionSpec)
{
	if (AddedActions.Contains(ActionSpec) && IsValid(ActionSpec.Action))
	{
		return ActionSpec.Action->RequestExecuteAction(GameplayActionActorInfo, ActionSpec.Level);
	}
	else
	{
		return false;
	}
}

bool UGameplayActionComponent::TryActivateActionsByTag(const FGameplayTagContainer& ActionTag)
{
	bool bActivatedAny = false;
	
	for (const FGameplayActionSpec ActionSpec : AddedActions)
	{
		if (IsValid(ActionSpec.Action) && ActionTag.HasTag(ActionSpec.Action->GetActionTag()))
		{
			ActionSpec.Action->RequestExecuteAction(GameplayActionActorInfo, ActionSpec.Level);
			
			bActivatedAny = true;
		}
	}
	
	return bActivatedAny;
}

void UGameplayActionComponent::PressInputID(int32 InputID)
{
	const FGameplayActionSpec* ActionSpec = AddedActions.FindByPredicate([InputID](const FGameplayActionSpec Spec)
	{
		return Spec.InputID == InputID;
	});

	if (ActionSpec && IsValid(ActionSpec->Action) && !ActionSpec->Action->IsActive())
	{
		ActionSpec->Action->RequestExecuteAction(GameplayActionActorInfo, ActionSpec->Level);
	}
}

void UGameplayActionComponent::ReleaseInputID(int32 InputID)
{
	const FGameplayActionSpec* ActionSpec = AddedActions.FindByPredicate([InputID](const FGameplayActionSpec Spec)
{
	return Spec.InputID == InputID;
});

	if (ActionSpec && IsValid(ActionSpec->Action))
	{
		ActionSpec->Action->RequestEndAction();
	}
}

void UGameplayActionComponent::InitActorInfo()
{
	FGameplayActionActorInfo ActorInfo;

	if (!GetOwner())
	{
		return;
	}
	
	ActorInfo.OwnerActor = GetOwner();
	
	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	
	GetOwner()->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
	
	ActorInfo.SkeletalMeshComponents.Append(SkeletalMeshComponents);
	
	ActorInfo.MovementComponent = GetOwner()->GetComponentByClass<UMovementComponent>();
	
	ActorInfo.ActionComponent = this;
	
	GameplayActionActorInfo = MakeShared<FGameplayActionActorInfo>(ActorInfo);
	
	GameplayTagContainer = MakeShared<FGameplayTagContainer>();
}

void UGameplayActionComponent::AddActiveAction(UGameplayAction* Action)
{
	const FGameplayActionSpec* ExistingActionSpec = AddedActions.FindByPredicate([Action](const FGameplayActionSpec& Spec)
	{
		return Spec.Action == Action;
	});
	
	if (Action && ExistingActionSpec && !ActiveActions.Contains(Action))
	{
		ActiveActions.Add(Action);
		
		OnActionExecuted.Broadcast(Action->GetActionTag());
	}
}

void UGameplayActionComponent::RemoveActiveAction(UGameplayAction* Action, bool bWasCanceled)
{
	if (Action)
	{
		ActiveActions.Remove(Action);
		
		OnActionEnded.Broadcast(Action->GetActionTag(), bWasCanceled);
	}
}

void UGameplayActionComponent::AddOwnedGameplayTag(const FGameplayTag& Tag)
{
	if (GameplayTagContainer.IsValid())
	{
		GameplayTagContainer->AddTag(Tag);
	}
}

void UGameplayActionComponent::AddOwnedGameplayTags(const FGameplayTagContainer& Tags)
{
	if (GameplayTagContainer.IsValid())
	{
		GameplayTagContainer->AppendTags(Tags);
	}
}

void UGameplayActionComponent::RemoveOwnedGameplayTags(const FGameplayTagContainer& Tags)
{
	if (GameplayTagContainer.IsValid())
	{
		GameplayTagContainer->RemoveTags(Tags);
	}
}

void UGameplayActionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitActorInfo();
}

void UGameplayActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GameplayActionActorInfo.Reset();
	
	GameplayTagContainer.Reset();

	for (UGameplayAction* ActiveAction : ActiveActions)
	{
		if (IsValid(ActiveAction))
		{
			ActiveAction->RequestEndAction();
		}
	}
	
	for (const FGameplayActionSpec& AddedAction : AddedActions)
	{
		if (IsValid(AddedAction.Action))
		{
			AddedAction.Action->MarkAsGarbage();
		}
	}
	
	ActiveActions.Empty();
	
	AddedActions.Empty();
}


