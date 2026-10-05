# Shared Firmware Candidate Workflow

Referenced by `QMK/AGENTS.md` and `ZMK/AGENTS.md`. This file holds the
candidate → QC → release discipline common to both firmware tracks.
Each AGENTS.md fills in its own project name, directory paths, artifact type,
and protected hardware behavior.

## Session start

Run `backlog task list` (or query the root Backlog pool) for the relevant
submodule prefix (`[QMK]` or `[ZMK]`) and review the active `<PROJECT>/`
documents before starting work, to align on current goals, status, and
open backlog items.

## Candidate and release discipline

- A candidate lives at the project's designated keymap path. Start each
  candidate from the current approved baseline, not from an unrelated
  platform's artifact or an editable Launcher/app state.
- Before asking for approval, create one record in `<PROJECT>/QC-IN_PROGRESS/`
  identifying the candidate commit/hash, baseline, every changed binding by
  position, every changed non-keymap source file (with purpose and safety
  impact, or an explicit statement that none changed), review verdict, and
  expected hardware checks.
- The release record compares the candidate side by side with the most
  recent QA-passed release (baseline path, revision, hash). Add the reviewer
  verdict, build-input proof, artifact hash, and observed hardware result to
  that same record as work advances — do not create separate manifest,
  review-summary, or QC-status documents that duplicate it.
- One senior review follows deterministic validation by default. A second
  independent review is required for three or more changed physical
  positions, layer ordering/structure, Fn access, boot/reset/recovery
  behavior, wireless controls, combos/tap-hold, or shared runtime code.
- Build only the reviewed candidate. Record the exact generated artifact
  path and SHA-256. A successful compile or transfer is not hardware QC.
- Before any flash, assign a revision identifier using semantic versioning where the final number is the build iteration (e.g., `v0.25.6` instead of `v0.25.1-build_6`). Append a root `CHANGELOG.md` entry: revision id, date, artifact path + SHA-256, source commit (when available), target unit, and a pending observed-result field. Do not log a build merely because it compiled.
- Promote only after the tester explicitly reports every required hardware
  assertion passed. Move (do not copy) the exact approved source, artifact,
  hash sidecar, and completed release record into
  `<PROJECT>/releases/<revision>/`. Verify the archived artifact hash against
  its sidecar and update the matching `CHANGELOG.md` entry with the archived
  path and observed hardware result.
- Every QC checklist is cumulative and deterministic: retain every
  applicable assertion from prior checklists, add assertions for each newly
  changed position or behavior, and give an explicit expected result per
  testable position. Treat each iteration as regression coverage unless the
  specification explicitly removes or changes an assertion.

## Task tracking and handoff discipline

All tasks, handoffs, and backlog items are tracked through the root
`backlog` CLI rather than local `.ai/` directories:
- Use project prefixes in titles (`[QMK]` or `[ZMK]`) to route items to the
  correct firmware submodule.
- Use `backlog task create`, `backlog task edit`, and `backlog task done` to
  manage task lifecycles, acceptance criteria, and handoffs.
- Record durable findings or architecture notes in the relevant `<PROJECT>/`
  documentation (e.g. `DESIGN/` or `QC-IN_PROGRESS/`), and release history
  in `CHANGELOG.md`.

## Session end

Do not create transcripts, narrative recaps, or documentation solely to
record a session. Persist only durable, approved records: decisions in the
authoritative specification or backlog; completed actions in the relevant QC
or release artifact; approved firmware evidence in `releases/` and
`CHANGELOG.md`.
