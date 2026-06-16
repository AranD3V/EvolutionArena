# Evolution Arena

A creature-breeding **auto-battler** built in **Unreal Engine 5**, shipping on
**Android and Windows** as co-primary V1 targets. Players design unique lifeforms
through genetics, breeding, and mutation, then send them to fight autonomously in
PvE/PvP arenas — success comes from creature design and breeding strategy, not
manual combat control.

The experience is **mobile-first** (portrait, touch) and scales up to desktop
(landscape, mouse/keyboard) from a single codebase.

> **Status:** Early development. The deterministic gameplay + economy core is
> implemented and tested headless in C++ (genetics → breeding → mutation → rarity
> → battle → ranked/rating → XP/levels/unlocks → save). What remains is largely
> **editor-bound** (UMG UI, packaging) or **externally blocked** (EOS credentials,
> an Android test device). See [docs/tracker.md](docs/tracker.md) for live status.

---

## Highlights

- **Genetics** — genomes express into derived stats via dominant/recessive/
  codominant alleles; integer-only math for cross-architecture determinism.
- **Breeding & mutation** — Mendelian per-locus inheritance with seeded, rare
  mutations and a rarity calculation.
- **Auto-battle** — turn-based 1v1 simulation driven by a go-for-the-kill AI,
  emitting an event timeline the visual layer can replay (sim is decoupled from
  rendering).
- **Ranked arena** — asynchronous matches against stored opponent "ghost"
  snapshots with an integer Elo-like rating, behind an abstract leaderboard
  service so EOS can drop in later.
- **Progression** — XP → levels and data-driven, level-gated unlocks.
- **Save** — versioned, atomically-written local saves with a migration hook.
- **Determinism is a release gate** — same inputs + seed produce identical
  results, enforced by an automation test in CI.

## Tech stack

- **Engine:** Unreal Engine **5.6** (C++ gameplay systems; Blueprint/UMG for UI).
- **Targets:** Android (arm64, Vulkan) + Windows (Win64), 60 FPS on mid-range
  hardware.
- **Online:** Epic Online Services (auth, cloud save, leaderboards), accessed
  only through wrapper interfaces.
- **Content:** Data-driven via DataTables / Data Assets — no balance values
  hardcoded in C++.

## Repository layout

```
EvolutionArena.uproject     UE5 project descriptor (Windows + Android)
Source/EvolutionArena/      C++ game module, grouped by system:
  Genetics/  Breeding/  Battle/  Save/  Online/  Progression/  Creature/
  Tests/                    UE Automation tests (one per system)
Data/                       Seed content as JSON (DT_BodyParts, DT_Genes,
                            DT_Mutations, DT_Unlocks) → imported to DataTables
Config/                     Engine/game .ini configuration
Scripts/RunTests.ps1        Build + headless test harness (CI entry point)
.github/workflows/ci.yml    CI: build + automation suite on a self-hosted runner
docs/                       Living design/spec documents (see below)
```

## Getting started

You need Unreal Engine 5.6 installed. Examples below use `<UE5>` for your engine
root (e.g. `D:\UE_5.6`).

```bash
# Generate IDE project files (after adding/removing C++ classes)
"<UE5>/Engine/Build/BatchFiles/Build.bat" -projectfiles -project="EvolutionArena.uproject" -game -rocket

# Build the editor target (Development, Win64)
"<UE5>/Engine/Build/BatchFiles/Build.bat" EvolutionArenaEditor Win64 Development -project="EvolutionArena.uproject"

# Open the editor
"<UE5>/Engine/Binaries/Win64/UnrealEditor.exe" "EvolutionArena.uproject"
```

> **Note:** on a machine where the launcher registry maps engine `5.6` to the
> wrong drive, double-clicking the `.uproject` may fail. Launch the editor with
> the explicit path above instead. CLI builds are unaffected.

## Tests

Simulation and economy logic must be tested, and the **battle-determinism** test
is a release gate that must stay green.

```bash
# Canonical: build + run the full suite with a CI pass/fail exit code
pwsh ./Scripts/RunTests.ps1                      # all EvolutionArena tests
pwsh ./Scripts/RunTests.ps1 -SkipBuild           # tests only (reuse binaries)
pwsh ./Scripts/RunTests.ps1 -Engine "C:\UE_5.6"  # point at a specific engine
pwsh ./Scripts/RunTests.ps1 -TestFilter EvolutionArena.Battle   # a subset

# Or run the automation suite headless directly
"<UE5>/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "EvolutionArena.uproject" \
  -ExecCmds="Automation RunTests EvolutionArena; Quit" -unattended -nopause -nullrhi
```

CI (`.github/workflows/ci.yml`) runs the same harness on a **self-hosted Windows
runner** with UE 5.6 (set the `UE5_ROOT` repo variable to the engine path).
GitHub-hosted runners can't run this because they don't ship Unreal Engine.

## Packaging (Android)

```bash
"<UE5>/Engine/Build/BatchFiles/RunUAT.bat" BuildCookRun -project="EvolutionArena.uproject" \
  -platform=Android -clientconfig=Development -cook -stage -package -archive
```

## Documentation

The project is spec-driven; these living documents are the source of truth.

| Doc | Answers |
|---|---|
| [docs/PRD.md](docs/PRD.md) | What we're building and **why** (scope) |
| [docs/TechSpec.md](docs/TechSpec.md) | **How** it's built (architecture, stack) |
| [docs/appflow.md](docs/appflow.md) | How a user **moves** through the app |
| [docs/design.md](docs/design.md) | What it **looks and feels** like |
| [docs/schema.md](docs/schema.md) | The **data model** |
| [docs/implementationplan.md](docs/implementationplan.md) | In what **order** we build |
| [docs/tracker.md](docs/tracker.md) | **Where we are right now** |
| [docs/rules.md](docs/rules.md) | **How we write code** here |

Conventions worth knowing before contributing: the battle simulation and genetics
must stay **fully deterministic** (seeded RNG only — no `FMath::Rand`, wall-clock,
or order-dependent float accumulation); balance values live in **DataTables**, not
code; online/save go **through their wrapper interfaces**; and new behaviour ships
with tests. See [docs/rules.md](docs/rules.md) for the full set.
