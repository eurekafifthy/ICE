// Copyright Yureka. All Rights Reserved.

using UnrealBuildTool;

public class InlineCodeEditor : ModuleRules
{
    public InlineCodeEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "InputCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "UnrealEd",
            "LevelEditor",
            "WorkspaceMenuStructure",
            "ToolMenus",
            "EditorStyle",
            "Projects"
        });
    }
}
