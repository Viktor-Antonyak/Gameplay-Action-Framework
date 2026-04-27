// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "AssetDefenitions/AssetDefinition_GameplayAttributeSet.h"
#include "GameplayAttributeSet.h"
#include "Blueprint/BlueprintSupport.h"

FText UAssetDefinition_GameplayAttributeSet::GetAssetDisplayName() const
{
	return FText::FromString(TEXT("Gameplay Attribute Set"));
}

FLinearColor UAssetDefinition_GameplayAttributeSet::GetAssetColor() const
{
	return FLinearColor(1.0f, 0.0f, 0.39f);
}

TSoftClassPtr<UObject> UAssetDefinition_GameplayAttributeSet::GetAssetClass() const
{
	return UGameplayAttributeSet::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_GameplayAttributeSet::GetAssetCategories() const
{
	static const auto Categories = { FAssetCategoryPath(NSLOCTEXT("AssetDefinitions", "GAF_Category", "Gameplay Action Framework")) };
	return Categories;
}
