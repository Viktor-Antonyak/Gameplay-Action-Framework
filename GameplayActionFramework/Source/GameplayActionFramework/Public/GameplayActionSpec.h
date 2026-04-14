#pragma once

#include "CoreMinimal.h"
#include "GameplayAction.h"
#include "GameplayActionSpec.generated.h"

/** 
 * Struct that defines a gameplay action specification. This struct is used to define the properties of a gameplay action that is added to the Gameplay Action Component. 
 * It contains a pointer to the Gameplay Action object, the level of the action, and the input ID associated with the action.
 * 
 * NOTE: Input ID will be got from the Gameplay Action's InputID property, so it is important to set the InputID property of the Gameplay Action class to the desired value before adding it to the Gameplay Action Component.
 */
USTRUCT(BlueprintType)
struct FGameplayActionSpec
{
	GENERATED_BODY()
	
public:
	
	/** Pointer to the Gameplay Action object. */
	UPROPERTY(BlueprintReadOnly, Category="Gameplay Action Spec")
	UGameplayAction* Action = nullptr;
	
	/** Level of the action. This is used to define the level of power or effectiveness of the action. */
	UPROPERTY(BlueprintReadOnly, Category="Gameplay Action Spec")
	int32 Level = -1;
	
	/** Input ID associated with the action. This can be used to trigger the action using input bindings. */
	UPROPERTY(BlueprintReadOnly, Category="Gameplay Action Spec")
	int32 InputID = -1;
	
	/** Equality operator for FGameplayActionSpec. Two FGameplayActionSpec are considered equal if they have the same Action pointer, Level, and InputID. */
	bool operator==(const FGameplayActionSpec& Other) const
	{
		return Action == Other.Action && Level == Other.Level && InputID == Other.InputID;
	}
	
	/** Inequality operator for FGameplayActionSpec. Two FGameplayActionSpec are considered not equal if they do not have the same Action pointer, Level, or InputID. */
	bool operator!=(const FGameplayActionSpec& Other) const
	{
		return !operator==(Other);
	}
};
