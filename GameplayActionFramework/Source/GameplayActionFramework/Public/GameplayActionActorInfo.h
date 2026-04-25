// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/MovementComponent.h"

#include "GameplayActionActorInfo.generated.h"

class UGameplayActionComponent;

/** 
 * Struct that defines the actor info for a gameplay action. This struct is used to cache references to the actor's components that are commonly used by gameplay actions. 
 * This is useful for performance reasons, as it avoids the need to repeatedly call GetComponentByClass or GetComponents in the gameplay action logic.
 */
USTRUCT(BlueprintType)
struct FGameplayActionActorInfo
{
	GENERATED_BODY()
	
	/** Weak pointer to the owner actor of the gameplay action. This is used to access the actor that owns the action. */
	UPROPERTY()
	TWeakObjectPtr<AActor> OwnerActor;
	
	/** Weak pointers to the skeletal mesh components of the owner actor. This is used to access the actor's skeletal mesh components. 
	 * 
	 * NOTE: This array is used instead of a single pointer because an actor can have multiple skeletal mesh components,
	 * for example, FPS character with separate skeletal mesh components for the body and the arms. In this case, both skeletal mesh components will be cached in this array.
	 */
	UPROPERTY()
	TArray<TWeakObjectPtr<USkeletalMeshComponent>> SkeletalMeshComponents;
	
	/** Weak pointer to the movement component of the owner actor. This is used to access the actor's movement component. 
	 * 
	 * NOTE: This is a weak pointer to the base UMovementComponent class, so if you need to access specific movement component functions, 
	 * you will need to cast it to the appropriate subclass (e.g., UCharacterMovementComponent).
	 */
	UPROPERTY()
	TWeakObjectPtr<UMovementComponent> MovementComponent;
	
	/** Weak pointer to the Gameplay Action Component of the owner actor. This is used to access the actor's Gameplay Action Component. */
	UPROPERTY()
	TWeakObjectPtr<UGameplayActionComponent> ActionComponent;
	
};
