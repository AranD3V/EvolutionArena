// Copyright Evolution Arena. All Rights Reserved.

using UnrealBuildTool;

public class EvolutionArena : ModuleRules
{
	public EvolutionArena(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Compile each .cpp as its own translation unit (no unity batching). The module
		// is small, and our many automation-test files intentionally reuse short
		// file-local helper names (MakePartTable, MakeFighter, …) in anonymous
		// namespaces; unity builds merge those namespaces and cause name collisions.
		bUseUnity = false;

		// Organize code by system in subfolders (Genetics/, Creature/, Battle/…)
		// and include them module-root-relative, e.g. #include "Genetics/Genome.h".
		// UE only auto-resolves same-folder includes, so the module root must be on
		// the include path explicitly.
		PublicIncludePaths.Add(ModuleDirectory);

		// Keep the public surface minimal; add modules per-system as features land
		// (see docs/TechSpec.md §4 and docs/rules.md "Project structure").
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}
