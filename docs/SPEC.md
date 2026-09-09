# Interactive Keymap Viewer — Platform Specification

**Status:** Draft, awaiting review.
**Current home:** `B1PRO/gh-pages/` in this (ZMK) repo. This is temporary — see
[§6 Multi-repo sharing](#6-multi-repo-sharing--deployment). Nothing here should
depend on that location.

## 1. Problem

`keymap_interactive.html` (2,347 lines) hardcodes one keyboard (Keychron B1
Pro), one firmware (ZMK), one revision's worth of behavior data, board
geometry, theme CSS, and render logic into a single file. That already needed
a refactor (see prior conversation). Three new requirements make "split into
files" insufficient on its own — the *data model* has to generalize first,
or the refactor just reproduces the same one-keyboard assumptions in more
files:

1. Support 3 more keyboards (Keychron V1 Max, a Dell laptop keyboard, an
   Apple M4 MacBook Pro keyboard) — all ANSI, but physically distinct.
2. Each keyboard's firmware behavior changes across revisions; the physical
   illustration does not. Users need to pick a revision and get a link to
   that revision's GitHub Release.
3. The page is shared across three separate firmware repos (this ZMK repo,
   an upcoming QMK repo, an upcoming Kanata repo), and per the existing
   backlog tickets, the intent is **one combined site**, not three
   independent deploys.

## 2. Requirements

**R1 — Multi-keyboard.** The engine must render any registered keyboard
profile without code changes: physical geometry, board artwork, and
layer/behavior data are all per-keyboard inputs, not assumptions baked into
`buildCellGroup`/`renderLayer`.

**R2 — Multi-revision.** Each (keyboard, firmware) pair has an ordered list
of revisions. Each revision carries its own layer/behavior data plus a link
to its GitHub Release (`https://github.com/{org}/{repo}/releases/tag/{tag}`).
Switching revisions swaps data only — never geometry or artwork.

**R3 — Multi-repo, single site.** ZMK, QMK, and Kanata (macOS + Linux)
variants are selectable from one deployed page, per the existing
`QMK: ... (combined with ZMK)` / `KANATA macOS: ... (combined with ZMK and
QMK)` backlog tickets.

## 3. Constraints discovered while scoping this

These aren't implementation details — they shape the data model, so they're
called out explicitly:

- **"ANSI" is a red herring for geometry reuse.** All four keyboards share
  the ANSI legend/glyph convention, but B1 Pro's hidden matrix (positions
  77–81, direct controls) and physical OS switch have no equivalent on a
  laptop-embedded keyboard. Each keyboard still needs a **full, independent**
  geometry definition. `layoutFamily: "ansi"` is descriptive metadata for
  rendering conventions (legend style, icon set), not a geometry shortcut.
- **Not every keyboard supports both OS layer-families.** The current data
  model assumes every keyboard has Mac *and* Win layer sets (B1 Pro is a
  hot-swap board with a physical switch). The MacBook Pro's built-in keyboard
  is Mac-only; the Dell laptop's is Windows-only. A keyboard profile must
  declare which platforms it supports, and the UI must hide the OS-switch
  control entirely for single-platform keyboards rather than disabling one
  side of it.
- **Firmware support varies per keyboard.** Nothing says V1 Max, the Dell
  keyboard, and the MacBook Pro will all eventually get ZMK *and* QMK *and*
  Kanata builds. The (keyboard × firmware) matrix is sparse and will stay
  sparse — the schema must not assume every keyboard has every firmware.
- **Three of four keyboards are asset-blocked, not code-blocked.** V1 Max,
  the Dell keyboard, and the MacBook Pro are waiting on layout-SVG permission
  from Keychron/Dell/Apple respectively. The registry must let a keyboard
  profile exist in a "pending assets" state — listed, selectable enough to
  show a "coming soon" state — without the engine special-casing "keyboards
  that don't have art yet." Do not build placeholder/fabricated artwork for
  these three; ship the picker entry disabled until real, permitted assets
  land.
- **No backend, on two different static hosts.** GitHub Pages and Cloudflare
  Pages both serve plain files over HTTPS with correct MIME types and no
  server-side logic. That's sufficient for `<script type="module">` +
  relative `import`s (no bundler required), but rules out anything needing a
  server: no API routes, no server-side redirects beyond each host's static
  config, no secrets in client code. Cross-repo data fetches (R3) must
  therefore be either build-time (baked into the deployed files) or
  client-side `fetch()` against a public, CORS-permitting URL — never a
  server call.
- **`file://` breaks ES modules.** Local verification needs a static server
  (`npx serve`, `python -m http.server`, etc.); opening the HTML file
  directly will fail on the module imports with a CORS error, which could
  easily be mistaken for a real bug during dev.
- **V1 Max hardware identity is still unresolved.** The existing ticket
  *"Confirm the on-hand V1 Max is the ANSI encoder model"* means we don't yet
  know for certain which physical V1 Max variant we're even building a
  profile for. This viewer's V1 Max work is downstream of that decision, not
  parallel to it.

## 4. Data model

```
manifest.json
{
  "keyboards": [
    { "id": "b1pro",   "name": "Keychron B1 Pro",        "layoutFamily": "ansi",
      "platforms": ["mac", "win"], "status": "available" },
    { "id": "v1max",   "name": "Keychron V1 Max",         "layoutFamily": "ansi",
      "platforms": ["mac", "win"], "status": "pending-assets" },
    { "id": "dell-laptop", "name": "Dell Laptop Keyboard","layoutFamily": "ansi",
      "platforms": ["win"], "status": "pending-assets" },
    { "id": "mbp-m4",  "name": "MacBook Pro (M4)",        "layoutFamily": "ansi",
      "platforms": ["mac"], "status": "pending-assets" }
  ]
}

keyboards/{id}/geometry.js        — GRID (physical key positions/sizes), per keyboard
keyboards/{id}/artwork/*.svg      — board illustration(s), per colorway if applicable
keyboards/{id}/firmware/{fw}/revisions.json
  — [{ "id": "v0.23", "label": "v0.23", "releaseUrl": "https://github.com/.../releases/tag/v0.23",
       "status": "released", "default": true },
     { "id": "v0.24", "label": "v0.24 (candidate)", "releaseUrl": "...", "status": "candidate" }]
keyboards/{id}/firmware/{fw}/{revisionId}/layers.js   — LAYERS data for that exact revision
```

`{fw}` is one of `zmk`, `qmk`, `kanata-macos`, `kanata-linux`. A keyboard's
`firmware/` directory only contains the firmware types that keyboard actually
has — sparse by design (see §3).

The rendering engine consumes exactly: one geometry object, one artwork set,
one `LAYERS` object, and the active keyboard's `platforms` list (to decide
whether to show an OS switch at all). It never reads `manifest.json` or
`revisions.json` directly — a thin "catalog" module resolves user picker
selections down to that same tuple the engine already knows how to render.
This is what makes R1/R2 additive: adding a keyboard or a revision is adding
files and a manifest entry, not changing the engine.

## 5. UI flow

Keyboard picker → (Firmware picker, **only shown when the keyboard has more
than one firmware entry**) → Revision picker (label + external link icon to
`releaseUrl`) → existing platform/layer tabs (suppressed if the keyboard
supports only one platform). Keyboards with `status: "pending-assets"` show
in the picker, disabled, with a "coming soon" label — never a fabricated or
placeholder illustration.

## 6. Multi-repo sharing & deployment

Three options, since R3 needs one site fed by three repos that don't share a
deploy pipeline today:

| | Approach | Pros | Cons |
|---|---|---|---|
| A | **Central repo + push-on-release.** A dedicated viewer repo; each firmware repo's release workflow pushes its `revisions.json`/`layers.js` into it. | Single URL, fully static, fast, works offline. | Needs a new repo, cross-repo write automation (PAT/deploy key) in QMK/Kanata repos that don't exist yet. |
| B | **Submodule per firmware repo.** Viewer code as a submodule; each firmware repo deploys its own Pages site with only its own data. | No cross-repo secrets; each repo self-contained. | Produces three separate sites, not the one combined site the existing tickets call for — would need extra cross-linking to feel unified. |
| C | **Runtime fetch.** One site fetches each firmware repo's tagged `revisions.json`/`layers.js` client-side via `raw.githubusercontent.com` at page load. | No push automation, always current, read-only (no secrets). | Extra network round-trips; must pin exact tags per revision; silently breaks if a source repo goes private. |

**Recommendation:** build the engine so the "catalog resolver" (§4) is the
only place that knows how data gets loaded, and start with local static
files (works today, in this repo, for B1 Pro). That keeps Option A/C both
open later without touching the engine. **Do not decide A vs. C now** — QMK
and Kanata repos don't exist yet, so there's no CORS/latency/rate-limit data
to decide on. Revisit when the first of those repos is created.

## 7. Explicit blockers (not engineering work, but gating it)

- Layout-SVG permission requests to Keychron (V1 Max), Dell, and Apple —
  outstanding, owner is the user, not an engineering task.
- V1 Max ANSI-encoder hardware confirmation — tracked separately, gates V1
  Max profile creation regardless of viewer readiness.
- QMK repo and Kanata repo do not exist yet — gates §6 decision and the
  QMK/Kanata firmware-integration tickets below.

## 8. Deliverables

Each maps to one Notion ticket (§ redefinition below):

1. This specification (review/approval).
2. Core engine refactor: HTML/CSS/JS module split, generalized to consume
   the §4 data tuple instead of hardcoded B1 Pro constants.
3. Keyboard/firmware/revision catalog + picker UI (§5), including the
   pending-assets disabled state.
4. B1 Pro ZMK data migration into the §4 file layout — the reference
   implementation, using the real v0.23 (released) and v0.24 (candidate)
   revisions already in this repo.
5. Static-hosting verification on both GitHub Pages and Cloudflare Pages
   (module loading, relative paths, no `file://` regressions).
6. Multi-repo sharing decision + implementation (§6) — blocked until a
   second firmware repo exists.
7. Vendor asset outreach tracker (Dell / Apple / Keychron V1 Max) — blocked,
   not engineering work, kept visible for planning.
