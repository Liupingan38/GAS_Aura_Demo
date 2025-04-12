// Copyright Liupingan

using UnrealBuildTool;
using System.Collections.Generic;

public class GAS_Aura_DemoTarget : TargetRules
{
	public GAS_Aura_DemoTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "GAS_Aura_Demo" } );
	}
}
