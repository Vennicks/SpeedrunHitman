// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HitmanDev : ModuleRules
{
	public HitmanDev(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem" });
	}
}
