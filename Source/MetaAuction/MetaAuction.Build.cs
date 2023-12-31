// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MetaAuction : ModuleRules
{
	public MetaAuction(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "MetaAuction.h";

		PublicIncludePaths.AddRange(new string[] { "MetaAuction" });

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
            "UMG",
            "glTFRuntime",  
            //"glTFRuntimeEditor",
			"ProceduralMeshComponent",
			"Json",
			//"JsonUtilities",
			"HTTP",
			"Stomp",
			"RHI",
			"RenderCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
      
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}