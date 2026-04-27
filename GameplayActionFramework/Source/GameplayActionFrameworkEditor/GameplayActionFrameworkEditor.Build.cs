using UnrealBuildTool;

public class GameplayActionFrameworkEditor : ModuleRules
{
    public GameplayActionFrameworkEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "GameplayActionFramework"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "PropertyEditor",
                "EditorStyle",
                "InputCore",
                "GraphEditor",
                "BlueprintGraph",
                "KismetWidgets",
                "KismetCompiler",
                "AssetTools",
                "AssetDefinition"
            }
        );
    }
}
