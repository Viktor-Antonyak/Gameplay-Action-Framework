// Copyright (c) 2026 Viktor Antonyak. All Rights Reserved. Gameplay Action Framework.

#pragma once

#include "Modules/ModuleManager.h"

class FGameplayActionFrameworkModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
