// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/UnrealType.h"
#include "GameplayAttributeSet.generated.h"

#define UE_API GAMEPLAYACTIONFRAMEWORK_API

class UGameplayAttributeSet;

/** 
 * Structure that holds the actual values of an attribute.
 * Contains both Base and Current values.
 */
USTRUCT(BlueprintType)
struct FGameplayAttributeData
{
	GENERATED_BODY()

public:
	FGameplayAttributeData()
		: BaseValue(0.f)
		, CurrentValue(0.f)
	{}

	FGameplayAttributeData(float DefaultValue)
		: BaseValue(DefaultValue)
		, CurrentValue(DefaultValue)
	{}

	float GetBaseValue() const { return BaseValue; }
	float GetCurrentValue() const { return CurrentValue; }

	void SetBaseValue(float NewValue) { BaseValue = NewValue; }
	void SetCurrentValue(float NewValue) { CurrentValue = NewValue; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	float BaseValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	float CurrentValue;
};

/** 
 * Describes a FGameplayAttributeData property inside an attribute set. 
 * Used for identifying and accessing attributes in a generic way.
 */
USTRUCT(BlueprintType)
struct FGameplayAttribute
{
	GENERATED_BODY()

public:
	FGameplayAttribute() : Attribute(nullptr) {}
	FGameplayAttribute(FProperty* InProperty);

	bool IsValid() const { return Attribute.Get() != nullptr; }
	FProperty* GetUProperty() const { return Attribute.Get(); }
	
	FString GetAttributeName() const;

	/** Returns the class of the attribute set that owns this attribute */
	UClass* GetAttributeSetClass() const
	{
		return Attribute.Get() ? Cast<UClass>(Attribute.Get()->GetOwner<UObject>()) : nullptr;
	}

	bool operator==(const FGameplayAttribute& Other) const { return Attribute == Other.Attribute; }
	bool operator!=(const FGameplayAttribute& Other) const { return Attribute != Other.Attribute; }

	friend uint32 GetTypeHash(const FGameplayAttribute& InAttribute)
	{
		return PointerHash(InAttribute.Attribute.Get());
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute", meta = (AllowPrivateAccess = "true"))
	TFieldPath<FProperty> Attribute;
};

/**
 * Base class for all attribute sets in the Gameplay Action Framework.
 */
UCLASS(Blueprintable, DefaultToInstanced, MinimalAPI)
class UGameplayAttributeSet : public UObject
{
	GENERATED_BODY()

public:
	UGameplayAttributeSet();

	/** Called just before an attribute's current value is changed. NewValue can be modified for clamping. */
	UFUNCTION(BlueprintNativeEvent, Category = "Attributes")
	void PreAttributeChange(const FGameplayAttribute& Attribute, UPARAM(ref) float& NewValue);
	virtual void PreAttributeChange_Implementation(const FGameplayAttribute& Attribute, UPARAM(ref) float& NewValue) {}

	/** Called just after an attribute's current value is changed. */
	UFUNCTION(BlueprintNativeEvent, Category = "Attributes")
	void PostAttributeChange(const FGameplayAttribute& Attribute, float NewValue, float OldValue);
	virtual void PostAttributeChange_Implementation(const FGameplayAttribute& Attribute, float NewValue, float OldValue) {}

	/** Returns current numeric value of an attribute */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetNumericValue(const FGameplayAttribute& Attribute) const;

	/** Sets current numeric value, triggering Pre/PostAttributeChange */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetNumericValue(const FGameplayAttribute& Attribute, float NewValue);

	/** Initializes base and current value without callbacks (usually for setup) */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void InitNumericValue(const FGameplayAttribute& Attribute, float NewValue);

protected:
	/** Internal helper to find the data struct for an attribute property */
	FGameplayAttributeData* GetAttributeData(const FGameplayAttribute& Attribute);
	const FGameplayAttributeData* GetAttributeData(const FGameplayAttribute& Attribute) const;
};

// --- Macros for easy attribute access ---

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	static FGameplayAttribute Get##PropertyName##Attribute() \
	{ \
		static FProperty* Prop = FindFieldChecked<FProperty>(ClassName::StaticClass(), GET_MEMBER_NAME_CHECKED(ClassName, PropertyName)); \
		return FGameplayAttribute(Prop); \
	} \
	FORCEINLINE float Get##PropertyName() const \
	{ \
		return PropertyName.GetCurrentValue(); \
	} \
	FORCEINLINE void Set##PropertyName(float NewVal) \
	{ \
		SetNumericValue(Get##PropertyName##Attribute(), NewVal); \
	} \
	FORCEINLINE void Init##PropertyName(float NewVal) \
	{ \
		PropertyName.SetBaseValue(NewVal); \
		PropertyName.SetCurrentValue(NewVal); \
	}

#undef UE_API
