# Design

> **Purpose:** Defines the look, feel, and design system — the visual and UX
> rules every interface follows.
> **Read it** before building or styling any UI.
> **Update it** whenever a token, component, or design rule changes.

- **Last updated:** 2026-06-13
- **Related:** `appflow.md` (structure of screens), `rules.md` (UI code rules)

---

## 1. Design principles & vibe

- **Creature first.** The lifeform is the hero of every screen; chrome stays out
  of its way. Genome and stats are always one tap from the creature.
- **Readable at a glance.** Mobile-first: portrait, large touch targets, legible
  without zooming. Scales up to Windows desktop (landscape/windowed) with pointer
  and keyboard affordances — never a stretched phone UI.
- **Bio-tech wonder.** A dark "lab" aesthetic with luminous, organic accents —
  evolution feels like discovery, not spreadsheets.
- **Celebrate rarity.** Mutations and rare breeds get distinct, earned visual
  payoff. Rarity is never communicated by color alone.
- **Fast and never blocking.** Battles are pre-simulated; UI animates results
  instead of making the player wait on logic.

## 2. Color

> Dark theme is the default and primary surface. Define tokens, not one-off hex
> values; reference tokens everywhere (Slate/UMG style assets).

| Token | Value | Usage |
|---|---|---|
| `--color-bg` | `#0B0F14` | App background (deep lab dark) |
| `--color-surface` | `#141B24` | Cards, panels, sheets |
| `--color-surface-raised` | `#1E2733` | Elevated/modal surfaces |
| `--color-text` | `#F2F6FA` | Primary text |
| `--color-text-muted` | `#93A1B0` | Secondary text, labels |
| `--color-primary` | `#34E0A1` | Primary actions, bio accent, links |
| `--color-secondary` | `#5B8DEF` | Secondary actions, info |
| `--color-success` | `#3DD68C` | Wins, positive deltas |
| `--color-warning` | `#F2B441` | Cooldowns, cautions |
| `--color-danger` | `#FF6B6B` | Losses, destructive actions |

**Rarity accents** (always paired with an icon/label, never color-only):

| Rarity | Token | Value |
|---|---|---|
| Common | `--rarity-common` | `#93A1B0` |
| Uncommon | `--rarity-uncommon` | `#3DD68C` |
| Rare | `--rarity-rare` | `#5B8DEF` |
| Epic | `--rarity-epic` | `#A86BFF` |
| Legendary | `--rarity-legendary` | `#F2B441` |

Dark mode: dark is the primary (and MVP-only) theme. Tokens are defined once; a
light variant is out of scope for V1.

## 3. Typography

- **Font family:** One geometric/humanist sans for UI (e.g. Inter / Rubik); a
  numeric/mono variant for stat readouts where alignment matters.
- **Scale (dp):** 12 / 14 / 16 / 20 / 24 / 32 / 40. Body 16; captions 12–14;
  screen titles 24–32; hero/reveal numbers 40.
- **Weights:** Regular (body), Medium (labels/buttons), Semibold/Bold (titles,
  emphasis, stat values).
- **Line height & tracking:** ~1.4 line height for body; slightly tight tracking
  on large titles. Keep stat numbers tabular for alignment.

## 4. Spacing, radius, elevation

- **Spacing scale (dp):** 4, 8, 12, 16, 24, 32, 48. Default screen padding 16;
  section gaps 24.
- **Radius:** sm 8 / md 12 / lg 20 / pill (full). Cards use md; sheets use lg;
  buttons use pill or md.
- **Elevation:** Flat-ish, lit by glow rather than heavy shadows. e0 base bg,
  e1 surface (subtle shadow), e2 raised/modal (stronger shadow + faint accent
  rim for rare items).

## 5. Components

> Inventory of reusable UMG widgets and the states each must support.

| Component | Variants | States | Notes |
|---|---|---|---|
| Button | primary / secondary / ghost / danger | default, hover, pressed, focused, disabled, loading | Min 48dp touch height; `hover`/`focused` cover mouse + keyboard nav on desktop; loading shows inline spinner |
| Creature Card | grid / list / hero | default, selected, locked, on-cooldown | Shows portrait, name, rarity badge, key stats |
| Stat Bar | bar / radial | default, buffed, debuffed | Tabular numbers; color-coded but labeled |
| Rarity Badge | common…legendary | default | Icon + label + accent; never color-only |
| Part Slot Picker | per body-part slot | empty, filled, invalid combo | Used in the Lab; live preview updates on change |
| Bottom Nav | 5 tabs | default, active | Persistent on hub-level screens |
| Modal / Sheet | dialog / bottom sheet | open, closing | Reveal, confirms, cost prompts |
| Result Banner | win / loss | enter, idle | Shows rating delta and rewards |
| Empty State | generic | default | Illustration + one clear CTA |
| Toast / Banner | info / offline / error | default | Offline banner persists while disconnected |

## 6. Layout & responsiveness

- **Orientation:** Portrait on phone (primary design target); landscape/windowed on
  Windows desktop. Layout adapts — desktop is not a stretched phone screen.
- **Breakpoints (dp width):** phone-sm < 360, phone 360–479, phone-lg 480–599,
  tablet ≥ 600, **desktop ≥ 900** (landscape; content gets a max readable width and
  may use multi-pane on hub/collection). Spacing/columns scale up per tier.
- **Grid / container:** Single-column content with a max readable width on tablet;
  Collection uses a responsive 2–4 column creature grid.
- **Safe areas / input:** Respect Android notches/gesture insets and keep primary
  actions within thumb reach (bottom third) on phone. On desktop, support a resizable
  window, pointer hover, and keyboard/gamepad focus traversal.
- **Layout patterns:** Hub + bottom nav for top-level; full-screen modals for
  reveal/battle; bottom sheets for confirms and details.

## 7. Accessibility

- **Target standard:** WCAG 2.1 AA where applicable to a game UI.
- **Color contrast:** ≥ 4.5:1 for body text, ≥ 3:1 for large text and meaningful
  UI; verify accent-on-dark combinations.
- **Touch targets & focus:** Minimum 48×48dp with adequate spacing on touch. On
  desktop, every interactive element is keyboard-reachable with a visible focus state
  and supports pointer hover.
- **Color independence:** Rarity, win/loss, and buff/debuff always carry an icon
  or label in addition to color (colorblind-safe).
- **Motion:** Respect "reduce motion"; battle replay and reveals offer a skip and
  a reduced-animation path.
- **Text scaling:** Honor OS font-size settings without clipping critical info.

## 8. Iconography & imagery

- **Icons:** A single consistent set, ~24dp, medium stroke, rounded to match the
  organic vibe. Stat and part-slot icons are part of the set.
- **Creature imagery:** Procedurally assembled from modular parts; visual variety
  comes from gene-driven color/scale/material variation, not bespoke art —
  reuse meshes aggressively (solo-dev / limited-art constraint).
- **Illustration / tone:** Bio-luminescent, lab-meets-wild. Backgrounds stay
  dark and low-detail so creatures pop and performance stays high.

## 9. References

- Brand/visual moodboard — [TBD link]
- UMG style assets / token mapping — [TBD link]
- Inspiration (auto-battlers, creature collectors) — [TBD links]
