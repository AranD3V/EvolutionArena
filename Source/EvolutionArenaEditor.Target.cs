// Copyright Evolution Arena. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class EvolutionArenaEditorTarget : TargetRules
{
	public EvolutionArenaEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		ExtraModuleNames.Add("EvolutionArena");
	}
}
