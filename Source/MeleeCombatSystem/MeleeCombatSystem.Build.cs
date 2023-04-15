// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MeleeCombatSystem : ModuleRules
{
	public MeleeCombatSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "GameplayTags", "UMG", "GameplayTasks" });
	}
}
