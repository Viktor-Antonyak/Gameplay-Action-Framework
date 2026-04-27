// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "AssetDefenitions/AssetDefinition_GameplayAction.h"
#include "GameplayAction.h"
#include "Blueprint/BlueprintSupport.h"

FText UAssetDefinition_GameplayAction::GetAssetDisplayName() const
{
	return FText::FromString(TEXT("Gameplay Action"));
}

FLinearColor UAssetDefinition_GameplayAction::GetAssetColor() const
{
	return FLinearColor(0.0f, 1.0f, 1.0f);
}

TSoftClassPtr<UObject> UAssetDefinition_GameplayAction::GetAssetClass() const
{
	return UGameplayAction::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_GameplayAction::GetAssetCategories() const
{
	static const auto Categories = { FAssetCategoryPath(NSLOCTEXT("AssetDefinitions", "GAF_Category", "Gameplay Action Framework")) };
	return Categories;
}

