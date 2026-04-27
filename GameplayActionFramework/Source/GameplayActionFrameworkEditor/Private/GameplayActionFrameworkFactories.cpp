// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "GameplayActionFrameworkFactories.h"
#include "GameplayAction.h"
#include "GameplayAttributeSet.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraph/EdGraph.h"

UGameplayActionFactory::UGameplayActionFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UGameplayAction::StaticClass();
}

UObject* UGameplayActionFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(UGameplayAction::StaticClass(), InParent, InName, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	
	if (NewBP)
	{
		UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(NewBP);
		if (EventGraph)
		{
			int32 NodeInstance = 0;
			
			// Add OnExecuteAction
			FKismetEditorUtilities::AddDefaultEventNode(NewBP, EventGraph, GET_FUNCTION_NAME_CHECKED(UGameplayAction, OnExecuteAction), UGameplayAction::StaticClass(), NodeInstance);
			
			// Add OnEndAction
			NodeInstance += 150; 
			FKismetEditorUtilities::AddDefaultEventNode(NewBP, EventGraph, GET_FUNCTION_NAME_CHECKED(UGameplayAction, OnEndAction), UGameplayAction::StaticClass(), NodeInstance);
		}
	}

	return NewBP;
}

UGameplayAttributeSetFactory::UGameplayAttributeSetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UGameplayAttributeSet::StaticClass();
}

UObject* UGameplayAttributeSetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FKismetEditorUtilities::CreateBlueprint(UGameplayAttributeSet::StaticClass(), InParent, InName, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
}
