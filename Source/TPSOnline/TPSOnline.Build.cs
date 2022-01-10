// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TPSOnline : ModuleRules
{
	public TPSOnline(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG" });
	}
}
