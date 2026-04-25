// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayActionInterface.generated.h"

#define UE_API GAMEPLAYACTIONFRAMEWORK_API

class UGameplayActionComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGameplayActionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that want to use the Gameplay Action Framework. It provides a function to get the Gameplay Action Component of the actor.
 */
class GAMEPLAYACTIONFRAMEWORK_API IGameplayActionInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Gameplay Action Interface")
	UGameplayActionComponent* GetGameplayActionComponent() const;
};

#undef UE_API
