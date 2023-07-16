// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAS : ModuleRules
{
	public GAS(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"GameplayAbilities", "GameplayTags", "GameplayTasks", 
			"PhysicsCore", "EnhancedInput", "MotionWarping", "NetCore"
		});
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
		
		PublicIncludePaths.AddRange(new string[] { "GAS" });
	}
}
