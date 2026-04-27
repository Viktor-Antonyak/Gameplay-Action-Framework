// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameplayAttributeSet.h"
#include "GameplayActionBlueprintLibrary.generated.h"

#define UE_API GAMEPLAYACTIONFRAMEWORK_API

class UGameplayActionComponent;
struct FGameplayAttribute;

/**
 * Blueprint function library for the Gameplay Action Framework.
 */
UCLASS()
class UGameplayActionBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Returns the Gameplay Action Component of the actor. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	static UGameplayActionComponent* GetGameplayActionComponent(AActor* Actor);

	/** Returns the owned gameplay tags of the actor. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	static FGameplayTagContainer GetOwnedGameplayTagsFromActor(AActor* Actor);
	
	/** Adds owned gameplay tags to the actor. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	static bool AddOwnedGameplayTagsToActor(AActor* Actor, FGameplayTagContainer Tags);

	/** Removes owned gameplay tags from the actor. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Action")
	static bool RemoveOwnedGameplayTagsFromActor(AActor* Actor, FGameplayTagContainer Tags);

	/** Returns the float value of an attribute from the actor. */
	UFUNCTION(BlueprintCallable, Category="Gameplay Attribute")
	static float GetAttributeValueFromActor(AActor* Actor, const FGameplayAttribute& Attribute);
};

#undef UE_API
