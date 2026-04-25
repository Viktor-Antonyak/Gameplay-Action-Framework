// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "GameplayActionComponent.h"
#include "GameplayAttributeSet.h"
#include "GameplayAction.h" 

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayActionComponent)

DEFINE_LOG_CATEGORY(LogGameplayActionComponent);

UGameplayActionComponent::UGameplayActionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGameplayActionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitActorInfo();
	InitAttributes();
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
	AttributeSets.Empty(); 
}

// --- Action Management ---
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

bool UGameplayActionComponent::TryActivateActionsByTag(UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTagContainer ActionTags)
{
	bool bActivatedAny = false;
	
	for (const FGameplayActionSpec& ActionSpec : AddedActions)
	{
		if (IsValid(ActionSpec.Action) && ActionSpec.Action->GetActionTag().MatchesAny(ActionTags))
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

// --- Tag Management ---
void UGameplayActionComponent::AddOwnedGameplayTag(FGameplayTag Tag)
{
	if (GameplayTagContainer.IsValid())
	{
		GameplayTagContainer->AddTag(Tag);
	}
}

void UGameplayActionComponent::AddOwnedGameplayTags(FGameplayTagContainer Tags)
{
	if (GameplayTagContainer.IsValid())
	{
		GameplayTagContainer->AppendTags(Tags);
	}
}

void UGameplayActionComponent::RemoveOwnedGameplayTags(FGameplayTagContainer Tags)
{
	if (GameplayTagContainer.IsValid())
	{
		GameplayTagContainer->RemoveTags(Tags);
	}
}

// --- Attribute Management ---
UGameplayAttributeSet* UGameplayActionComponent::GetAttributeSetByClass(TSubclassOf<UGameplayAttributeSet> AttributeClass) const
{
	if (!AttributeClass) return nullptr;

	for (UGameplayAttributeSet* Set : AttributeSets)
	{
		// FIX: Use IsA to support subclasses
		if (IsValid(Set) && Set->IsA(AttributeClass))
		{
			return Set;
		}
	}
	return nullptr;
}

float UGameplayActionComponent::GetAttributeValue(const FGameplayAttribute& Attribute) const
{
	if (!Attribute.IsValid())
	{
		return 0.0f;
	}

	const UGameplayAttributeSet* AttributeSet = GetAttributeSetByClass(Attribute.GetAttributeSetClass());

	if (AttributeSet == nullptr)
	{
		return 0.0f;
	}
	
	return AttributeSet->GetNumericValue(Attribute);
}

void UGameplayActionComponent::SetAttributeValue(const FGameplayAttribute& Attribute, float NewValue)
{
	if (!Attribute.IsValid())
	{
		return;
	}

	UGameplayAttributeSet* AttributeSet = GetAttributeSetByClass(Attribute.GetAttributeSetClass());

	if (AttributeSet)
	{
		AttributeSet->SetNumericValue(Attribute, NewValue);
	}
}

UGameplayAttributeSet* UGameplayActionComponent::AddAttributeSet(TSubclassOf<UGameplayAttributeSet> AttributeSetClass)
{
	if (!AttributeSetClass)
	{
		return nullptr;
	}
	
	if (UGameplayAttributeSet* ExistingSet = GetAttributeSetByClass(AttributeSetClass))
	{
		return ExistingSet;
	}

	UGameplayAttributeSet* NewAttributeSet = NewObject<UGameplayAttributeSet>(GetOwner(), AttributeSetClass);
	AttributeSets.Add(NewAttributeSet);
	return NewAttributeSet;
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

void UGameplayActionComponent::InitAttributes()
{
	// Create default attribute sets
	for (const TSubclassOf<UGameplayAttributeSet>& AttributeClass : DefaultAttributes)
	{
		if (AttributeClass)
		{
			AddAttributeSet(AttributeClass); 
		}
	}

	// Apply initial attribute values
	for (const FAttributeInitializationData& InitData : InitialAttributeValues)
	{
		if (!InitData.Attribute.IsValid())
		{
			continue;
		}
		
		UGameplayAttributeSet* AttributeSet = GetAttributeSetByClass(InitData.Attribute.GetAttributeSetClass());

		if (AttributeSet)
		{
			AttributeSet->InitNumericValue(InitData.Attribute, InitData.InitialValue);
		}
	}
}
