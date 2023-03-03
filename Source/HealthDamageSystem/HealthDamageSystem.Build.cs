// Copyright shenkns Health-Damage System Developed With Unreal Engine. All Rights Reserved 2022.

using UnrealBuildTool;

public class HealthDamageSystem : ModuleRules
{
	public HealthDamageSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateIncludePaths.AddRange(
			new string[] 
			{
				"HealthDamageSystem/Public/"
			}
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"NetCore",
				"DataSystem",
				"LogSystem"
			}
		);
		
		PrivateIncludePathModuleNames.AddRange(
			new string[]
			{
				"DataSystem",
				"LogSystem"
			}
		);
	}
}
