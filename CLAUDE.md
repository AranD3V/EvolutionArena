# CLAUDE.md

> This file is loaded automatically at the start of every Claude Code session.
> It is the **control center** for the project. It tells you (Claude) what the
> project is, where everything lives, and how to work so that nothing drifts
> away from the plan.

---

## 1. Project at a glance

- **Name:** Evolution Arena
- **One-liner:** A mobile-first (Android) Unreal Engine 5 creature-breeding
  auto-battler — players design unique lifeforms through genetics, breeding, and
  mutation, then compete in autonomous PvE/PvP arenas.
- **Status:** Planning → early development (doc set drafted; M0 setup is next)
- **Owner:** Solo developer

For the full picture, the source of truth is the document set below — not this
summary. Keep this section short.

---

## 2. The document system

This project uses a small set of living documents. Each one answers a different
question, and each is read and updated at a specific point in the workflow.
**Do not guess** at product, technical, or design decisions that are already
written down — read the relevant file first.

| Document | What it answers | You READ it when… | You UPDATE it when… |
|---|---|---|---|
| `docs/PRD.md` | What are we building and **why**? | Starting any new feature; unsure if something is in scope | Scope, goals, or requirements change |
| `docs/TechSpec.md` | **How** is it built (architecture, stack)? | Designing or changing system structure | A technical decision is made or revised |
| `docs/appflow.md` | How does a **user move** through the app? | Building navigation, screens, or flows | A flow or screen behaviour changes |
| `docs/design.md` | What does it **look and feel** like? | Building or styling any UI | A design token, component, or rule changes |
| `docs/schema.md` | What is the **data model**? | Touching the database, models, or queries | A table, field, relationship, or index changes |
| `docs/implementationplan.md` | In what **order** do we build it? | Planning work; picking the next task | The plan, milestones, or sequencing change |
| `docs/tracker.md` | **Where are we right now**? | **Start of every session** | **After every task** — always keep current |
| `docs/rules.md` | **How do we write code** here? | Always (auto-imported below) | A convention is added or changed |

> Layout for this project: `CLAUDE.md` lives at the **repo root** (where Claude
> Code auto-loads it); all other living docs live in **`docs/`**. Paths below and
> the import at the bottom are written accordingly.

---

## 3. How to work (the loop)

The workflow runs in phases. Most sessions live inside the **Build → Track**
loop, but always respect the planning docs that came before.

```
  Plan                Design               Build                Track
┌────────┐         ┌──────────┐         ┌────────┐          ┌──────────┐
│ PRD    │  ──▶    │ appflow  │  ──▶    │  code  │   ──▶    │ tracker  │
│TechSpec│         │ design   │         │ against│          │ (update  │
│ schema │         │          │         │ specs  │ ◀──loop──│  every   │
└────────┘         └──────────┘         └────────┘          │  task)   │
                                                            └──────────┘
        docs/implementationplan.md sequences all of this.
        docs/rules.md governs every line of code.
```

### Session start routine (do this first, every time)

1. **Read `docs/tracker.md`** to load current state: what's done, what's in
   progress, what's blocked, and what's next.
2. Glance at `docs/implementationplan.md` to confirm the next task and its
   acceptance criteria.
3. Read any spec relevant to the task (see the table above) **before** writing
   code. Only load what you need — don't read everything every time.

### Before you write code

- The rules in `docs/rules.md` are already in context (imported). **Follow them.**
- Building UI? Read `docs/design.md` **and** `docs/appflow.md` first.
- Touching data, models, or queries? Read `docs/schema.md` first.
- Changing architecture, adding a service, or picking a library? Check
  `docs/TechSpec.md` first, and record the decision there.

### After you finish a task

1. **Update `docs/tracker.md`**: move the item to its new state, note any
   decisions made, and record blockers or follow-ups.
2. If you changed the data model → update `docs/schema.md`.
3. If you changed a flow or screen → update `docs/appflow.md`.
4. If you made or revised a technical decision → update `docs/TechSpec.md`.
5. Keep docs and code in sync. A change that isn't reflected in the docs is a
   bug in the documentation.

### When requirements or scope change

- **Do not silently diverge from the PRD.** If a request conflicts with or
  extends `docs/PRD.md`, say so, then update the PRD (and any downstream doc)
  before or alongside the work.
- Don't invent features that aren't in the PRD. If something seems missing,
  flag it and propose adding it rather than quietly building it.

---

## 4. Operating principles

- **Specs are the source of truth.** When the code and a doc disagree, stop and
  resolve it — don't assume the code is right.
- **Plan before building.** For anything non-trivial, confirm the relevant docs
  exist and are current before generating code.
- **Small, reviewable steps.** Prefer the next task in the plan over large,
  speculative changes.
- **Ask when genuinely ambiguous.** A short clarifying question beats a wrong
  assumption baked into the codebase.
- **Keep docs lean.** Each doc stays focused and ideally under ~200 lines. Long
  files reduce how reliably instructions are followed.

---

## 5. Tech stack & commands

> Fill this in. Claude Code follows commands far more reliably when they're
> listed explicitly here. Keep it to what you actually run.

- **Stack:** Unreal Engine 5 — C++ gameplay systems + Blueprint/UMG UI. Target
  Android (arm64, Vulkan), 60 FPS on mid-range devices. Online via Epic Online
  Services (auth, cloud save, leaderboards). Content is data-driven via
  DataTables / Data Assets.
- **Build tooling:** UnrealBuildTool + RunUAT. Project file:
  `EvolutionArena.uproject`. (Project/targets are created in M0 — see
  `docs/implementationplan.md`. `<UE5>` below = your engine install path.)

```bash
# Generate IDE project files (after adding/removing C++ classes)
"<UE5>/Engine/Build/BatchFiles/Build.bat" -projectfiles -project="EvolutionArena.uproject" -game -rocket

# Open the editor
"<UE5>/Engine/Binaries/Win64/UnrealEditor.exe" "EvolutionArena.uproject"

# Build the editor target (Development, Win64)
"<UE5>/Engine/Build/BatchFiles/Build.bat" EvolutionArenaEditor Win64 Development -project="EvolutionArena.uproject"

# Build + run the full automation suite with a CI pass/fail exit code (canonical)
#   pwsh ./Scripts/RunTests.ps1 [-Engine <UE root>] [-SkipBuild] [-TestFilter EvolutionArena.Battle]

# Run automation tests headless directly (includes the battle-determinism release gate)
"<UE5>/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "EvolutionArena.uproject" -ExecCmds="Automation RunTests EvolutionArena; Quit" -unattended -nopause -nullrhi

# Package an Android build (Development)
"<UE5>/Engine/Build/BatchFiles/RunUAT.bat" BuildCookRun -project="EvolutionArena.uproject" -platform=Android -clientconfig=Development -cook -stage -package -archive
```

---

## 6. Always-on rules

The coding rules apply to every change, so they're imported here and loaded
each session:

@docs/rules.md

> Everything else in the table is read **on demand** at the right phase — this
> keeps the session context lean. If you'd rather always load the current
> status too, add `@docs/tracker.md` below this line.
