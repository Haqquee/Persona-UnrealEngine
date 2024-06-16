// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NPC_Chatbot_Demo : ModuleRules
{
	public NPC_Chatbot_Demo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "HTTP", "Json", "UMGEditor",
        "UnrealEd", "SlateCore"});
	}
}
