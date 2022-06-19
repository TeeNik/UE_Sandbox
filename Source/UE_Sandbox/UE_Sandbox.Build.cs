// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE_Sandbox : ModuleRules
{
	public UE_Sandbox(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
