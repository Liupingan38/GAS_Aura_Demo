// Copyright Liupingan

using UnrealBuildTool;
using System.Collections.Generic;

public class GAS_Aura_DemoEditorTarget : TargetRules
{
	public GAS_Aura_DemoEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "GAS_Aura_Demo" } );
	}
}
