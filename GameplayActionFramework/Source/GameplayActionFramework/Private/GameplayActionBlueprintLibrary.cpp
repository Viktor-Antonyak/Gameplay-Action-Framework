// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "GameplayActionBlueprintLibrary.h"
#include "GameplayActionInterface.h"
#include "GameplayActionComponent.h"
#include "GameplayAttributeSet.h"
#include "InstancedStruct.h"

UGameplayActionComponent* UGameplayActionBlueprintLibrary::GetGameplayActionComponent(AActor* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UGameplayActionInterface>())
	{
		return IGameplayActionInterface::Execute_GetGameplayActionComponent(Actor);
	}
	return nullptr;
}

FGameplayTagContainer UGameplayActionBlueprintLibrary::GetOwnedGameplayTagsFromActor(AActor* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UGameplayActionInterface>())
	{
		const UGameplayActionComponent* Component = IGameplayActionInterface::Execute_GetGameplayActionComponent(Actor);
		if (IsValid(Component))
		{
			return Component->GetOwnedGameplayTags();
		}
	}
	return FGameplayTagContainer();
}

bool UGameplayActionBlueprintLibrary::AddOwnedGameplayTagsToActor(AActor* Actor, FGameplayTagContainer Tags)
{
	if (IsValid(Actor) && Actor->Implements<UGameplayActionInterface>())
	{
		UGameplayActionComponent* Component = IGameplayActionInterface::Execute_GetGameplayActionComponent(Actor);
		if (IsValid(Component))
		{
			Component->AddOwnedGameplayTags(Tags);
			return true;
		}
	}
	return false;
}

bool UGameplayActionBlueprintLibrary::RemoveOwnedGameplayTagsFromActor(AActor* Actor, FGameplayTagContainer Tags)
{
	if (IsValid(Actor) && Actor->Implements<UGameplayActionInterface>())
	{
		UGameplayActionComponent* Component = IGameplayActionInterface::Execute_GetGameplayActionComponent(Actor);
		if (IsValid(Component))
		{
			Component->RemoveOwnedGameplayTags(Tags);
			return true;
		}
	}
	return false;
}

float UGameplayActionBlueprintLibrary::GetAttributeValueFromActor(AActor* Actor, const FGameplayAttribute& Attribute)
{
	if (IsValid(Actor) && Actor->Implements<UGameplayActionInterface>())
	{
		UGameplayActionComponent* Component = IGameplayActionInterface::Execute_GetGameplayActionComponent(Actor);
		if (IsValid(Component))
		{
			return Component->GetAttributeValue(Attribute);
		}
	}
	return 0.0f;
}
