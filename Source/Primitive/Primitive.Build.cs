// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Primitive : ModuleRules
{
	public Primitive(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "UMG", "SlateCore", "Json", "JsonUtilities" });
	}
}
