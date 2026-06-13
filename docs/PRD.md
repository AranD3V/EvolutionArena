# PRD — Product Requirements Document

> **Purpose:** Defines what we're building and why. This is the source of truth for scope. If something isn't here, it isn't in scope yet.

* **Project:** Evolution Arena
* **Last Updated:** 2026-06-13
* **Status:** Draft

---

# 1. Overview

Evolution Arena is a competitive creature breeding and auto-battler game built using Unreal Engine 5, shipping on **Android and Windows as co-primary platforms** for V1. The design is mobile-first (portrait, touch) and scales up to desktop (landscape, mouse/keyboard). Players create unique lifeforms through genetics, breeding, mutations, and evolution. These creatures fight autonomously in PvE and PvP arenas, where success is determined by strategic creature design rather than direct combat control.

The game combines creature collection, evolution simulation, genetics, and competitive ranked gameplay into a highly replayable progression experience.

---

# 2. Problem Statement

Most creature collection games rely on static characters, repetitive upgrades, and manual combat. Players rarely feel ownership over the creatures they use because every player ultimately possesses the same units.

Evolution Arena solves this problem by allowing players to create and evolve entirely unique species. Every creature is generated from genetic traits, body parts, and mutations, creating a system where player creativity and breeding strategy directly influence success.

Without this system:

* Progression becomes repetitive.
* Collection loses meaning.
* Competitive depth decreases.
* Long-term retention suffers.

The goal is to create a game where discovering and breeding powerful creatures becomes the primary source of progression and engagement.

---

# 3. Goals & Non-Goals

## Goals

* Build a deep and rewarding creature breeding system.
* Create competitive auto-battler gameplay.
* Support thousands of creature combinations through modular genetics.
* Encourage experimentation and discovery.
* Deliver a polished experience on both Android (portrait, touch) and Windows (landscape, mouse/keyboard) from one codebase.
* Establish long-term progression through evolution and rankings.

## Non-Goals

* Real-time player-controlled combat.
* Open-world exploration.
* MMORPG features.
* Complex machine learning systems.
* AAA photorealistic graphics.
* iOS, macOS, or console releases (V1 ships Android + Windows only).

---

# 4. Target Users

| Persona            | Description                                | Primary Need                 |
| ------------------ | ------------------------------------------ | ---------------------------- |
| Casual Collector   | Enjoys discovering creatures and mutations | Collection and progression   |
| Competitive Player | Enjoys rankings and optimization           | Winning ranked battles       |
| Breeder            | Loves experimentation and genetics         | Creating unique species      |
| Mobile Gamer       | Plays in short daily sessions              | Quick and rewarding gameplay |
| Desktop Player     | Plays on Windows with mouse/keyboard       | Comfortable PC controls, longer sessions |

---

# 5. Features & User Stories

## Must-Have (MVP)

### Creature Creation System

**User Story**

As a player, I want to build creatures from different body parts so that I can create unique species.

**Acceptance Criteria**

* Creature assembly system exists.
* Body parts affect stats.
* Players can preview results before creation.

---

### Genetics System

**User Story**

As a player, I want creatures to have inherited traits so that breeding decisions matter.

**Acceptance Criteria**

* Genetic values are stored.
* Traits are inherited.
* Genetics affect gameplay statistics.

---

### Breeding System

**User Story**

As a player, I want to breed creatures so that I can discover stronger offspring.

**Acceptance Criteria**

* Two creatures can breed.
* Offspring inherits parent traits.
* New offspring is generated dynamically.

---

### Mutation System

**User Story**

As a player, I want rare mutations so that every breeding attempt feels exciting.

**Acceptance Criteria**

* Mutation chances exist.
* Rare traits are generated.
* Mutations affect gameplay.

---

### Auto Battle System

**User Story**

As a player, I want creatures to battle automatically so that strategy occurs before combat begins.

**Acceptance Criteria**

* AI controls creatures.
* Battles complete without player input.
* Results depend on stats and decisions.

---

### Ranked Arena

**User Story**

As a player, I want ranked competition so that I can compare my creatures against others.

**Acceptance Criteria**

* Ranking system exists.
* Leaderboards update correctly.
* Match results affect rankings.

---

### Save System

**User Story**

As a player, I want my progress saved so that I can continue later.

**Acceptance Criteria**

* Creature data is persisted.
* Rankings are saved.
* Evolution progress is retained.

---

## Should-Have

### Evolution Tree

Players unlock advanced genetic branches and species.

### Squad Battles

Players can deploy multiple creatures simultaneously.

### Environment Modifiers

Different arenas provide unique bonuses and penalties.

### Daily Challenges

Daily objectives encourage player retention.

---

## Could-Have (Future)

### Creature Marketplace

Players trade creatures, eggs, and rare mutations.

### Seasonal Ranked Leagues

Competitive resets with exclusive rewards.

### Guilds / Clans

Social progression and cooperative competition.

### Spectator Mode

Watch battles between other players.

---

## Won't-Have (Version 1)

* Open world gameplay
* Manual combat
* Guild systems
* Trading systems
* Marketplace
* iOS / macOS / console releases
* Clan wars

---

# 6. Success Metrics

## Retention

* D1 Retention > 40%
* D7 Retention > 20%
* D30 Retention > 10%

## Engagement

* Average Session Length > 12 minutes
* Average Daily Battles > 5
* Average Weekly Breeding Attempts > 10

## Progression

* 70% of players breed at least one creature
* 50% of players participate in ranked battles

## Competitive

* 30% of active players engage with ranked mode weekly
* Average player owns 20+ creatures

---

# 7. Constraints & Assumptions

## Constraints

### Platform

* Android + Windows — co-primary targets for V1 (single codebase)
* Unreal Engine 5
* Mobile-first UX, scaled up to desktop

### Team

* Solo Developer
* Limited Art Budget
* Heavy Reliance on System-Generated Content

### Timeline

* MVP: 3–4 Months
* Alpha: 6 Months

### Performance

* Target 60 FPS
* Mid-range Android device + Windows desktop support

---

## Assumptions

* Players enjoy breeding-based progression.
* Auto-battlers remain popular.
* Procedural content can reduce development cost.
* Competitive rankings improve retention.
* Creature customization drives engagement.

---

# 8. Dependencies

## Technology

* Unreal Engine 5
* Epic Online Services (EOS) — works on both Android and Windows
* Android SDK + Windows build toolchain (Visual Studio / Win64)

## Backend

* Authentication Service
* Cloud Save Service
* Leaderboard Service

## Future Systems

* Trading Service
* Marketplace Backend
* Seasonal Event Framework

---

# 9. Open Questions

* [x] Should battles be simulated client-side or server-side? — **Client-side deterministic sim for MVP**; server re-validation post-MVP. (TechSpec §7)
* [ ] How many mutations should exist at launch? — *(content TBD; 3 seeded so far)*
* [x] What is the ideal breeding cooldown? — **4h MVP default** (`BreedCooldownHours`), tunable.
* [x] How should rarity be calculated? — **Σ part rarities + 2×mutations → thresholds** (MVP). (TechSpec §7)
* [ ] What should be the maximum creature level? — *(open; XP→level curve is linear, no cap yet)*
* [x] Should ranked battles be asynchronous? — **Yes, async vs opponent snapshots ("ghosts")**. (TechSpec §7)
* [ ] Is a marketplace required before launch? — *(no; Won't-Have for V1)*
* [ ] How many creature parts should be available in MVP? — *(content TBD; 5 seeded so far)*

---

# MVP Scope

## Included

* Creature Creation
* Genetics System
* Breeding
* Mutations
* Auto Battles
* AI Decision Making
* Ranked Arena
* Progression System
* Cloud Saves
* Android + Windows builds

## Excluded

* Marketplace
* Trading
* Guilds
* Clan Wars
* Seasonal Events
* iOS / macOS / console builds

---

# Vision Statement

Build the ultimate evolution-based strategy game where players create unique species, discover powerful genetic combinations, and dominate the evolutionary ladder through breeding, adaptation, and intelligent design.
