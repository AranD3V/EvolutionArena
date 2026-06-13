# Rules

> **Purpose:** The coding rules and conventions for this project. This file is
> imported into `CLAUDE.md`, so it applies to **every** change. Keep it short,
> specific, and high-signal — avoid vague advice like "write good code."
> **Update it** whenever a convention is added or changed.

- **Last updated:** 2026-06-13

---

## Core principles

- Follow the specs. Product decisions live in `PRD.md`, technical decisions in
  `TechSpec.md`, the data model in `schema.md`. Don't contradict them; if a
  change is needed, update the doc.
- Match the existing project. Mirror nearby patterns, module layout, and Unreal
  conventions before introducing new ones.
- Prefer small, focused changes over large speculative ones.
- Don't add features, plugins, or abstractions that aren't needed for the current
  task.

## Code style

- **Languages:** C++ for gameplay systems (genetics, breeding, mutation, battle
  sim, AI, save); Blueprint/UMG for UI and high-level glue. Don't put balance or
  simulation logic in Blueprint.
- **Engine conventions:** Follow the Unreal Engine coding standard (brace style,
  `const` correctness, `TObjectPtr`/smart pointers, `UPROPERTY`/`UFUNCTION`
  reflection where needed).
- **Formatting:** Match the engine standard; don't hand-reflow existing files.
- **Types:** Use engine types (`int32`, `FString`, `TArray`, `FName`, `FGuid`);
  avoid raw `new`/`delete` — use UObject lifetime / smart pointers.

## Naming (Unreal prefixes — enforce these)

- Classes: `A` actors, `U` UObjects/subsystems, `F` plain structs, `E` enums,
  `I` interfaces, `T` templates. e.g. `UGeneticsSubsystem`, `FGenome`, `ERarity`.
- Variables / functions: PascalCase (`DerivedStats`, `ComputeStats()`); booleans
  prefixed `b` (`bIsOnCooldown`).
- Constants: `UPPER_SNAKE_CASE` or `constexpr` PascalCase per engine norm.
- DataTable row ids: descriptive `FName`s, stable — renaming a row is a migration.
- Be descriptive; avoid abbreviations and single letters except loop indices.

## Project structure

- Gameplay C++ lives in the project `Source/` module(s), grouped by system
  (Genetics, Breeding, Battle, Save, Online). One clear responsibility per class.
- UI (UMG), DataTables, and Data Assets live under `Content/` in mirrored folders.
- Static content (parts, genes, mutations, balance) lives in **DataTables/Data
  Assets**, never hardcoded in C++.
- Automation tests live alongside their module under a `Tests/` folder.

## Stack-specific rules

- **Determinism (critical):** The battle simulation and genetics must be fully
  deterministic. Use only the seeded RNG stream; no `FMath::Rand`, no wall-clock,
  no frame-time, no order-dependent float accumulation in sim/genetics. Same
  inputs + seed → identical result, always.
- **Data-driven balance:** No magic numbers for stats, rates, costs, or cooldowns
  in code — read them from DataTables/config so content ships without recompiles.
- **UI uses design tokens:** Pull colors/spacing/typography from the `design.md`
  token set (style assets); no hard-coded hex or sizes in widgets.
- **Online behind interfaces:** All EOS calls go through wrapper interfaces so the
  game runs offline in-editor and the backend can be swapped later. No EOS calls
  scattered through gameplay code.
- **Save through the subsystem:** All persistence goes through the Save/Load
  subsystem; never serialize player data ad hoc. Bump `SaveVersion` on breaking
  changes and provide a migration step.
- **Mobile performance:** Respect mesh/material/draw-call budgets; the battle sim
  runs faster than real time and the visual layer replays it — don't couple
  simulation to the render tick.
- **Errors:** Handle and surface errors (failed login, save conflict, missing
  DataTable row); never swallow them silently.

## Security

- **Never commit secrets** — EOS client credentials, keystores, tokens. Use
  environment/config outside the repo; keep keystores out of source control.
- Never log PII or tokens.
- Validate external/loaded data: DataTable rows and loaded saves are validated at
  startup/load; reject or migrate malformed data rather than trusting it.
- Client battle results are trusted in MVP (documented risk) — plan server
  re-validation before any high-stakes competitive release.

## Testing

- Business/simulation logic must be tested: genetics expression, breeding
  inheritance, mutation rates (statistical), rarity calc, and battle determinism.
- Use UE Automation tests; run via CI (see `CLAUDE.md` commands).
- The battle-determinism reproduction test must stay green — it's a release gate.
- New behaviour ships with tests; bug fixes ship with a regression test.
- Don't mark a task done with failing tests.

## Git & commits

- Conventional Commits: `type(scope): summary` (e.g. `feat(breeding): add allele
  selection`).
- Small, atomic commits with a clear message.
- Don't commit commented-out code, debug logging, or large binary content that
  belongs in LFS/asset management.

## Documentation & sync (important)

- After completing a task, **update `tracker.md`**.
- When the data model changes, **update `schema.md`** in the same change.
- When a flow or screen changes, **update `appflow.md`**.
- When a technical decision is made or revised, **record it in `TechSpec.md`**.
- A change that isn't reflected in the docs isn't finished.

## Anti-patterns (don't)

- Don't silently diverge from the PRD — flag scope changes first.
- Don't introduce non-determinism into the simulation.
- Don't hardcode balance values that belong in DataTables.
- Don't put simulation/balance logic in Blueprint.
- Don't scatter EOS or save calls through gameplay code — go through the wrappers.
- Don't leave TODOs without an entry in `tracker.md`.
