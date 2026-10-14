# QC In Progress

This directory contains exact, versioned candidates that have not yet passed
hardware QC. Never overwrite a candidate after its hash has been reviewed.

For each newly defined keymap, create one companion `*.delivery-report.md`
record. That record accumulates the baseline comparison, changed zero-based
positions, behavior and code-file changes, direct-control result, review
verdict, build-input proof, UF2 hash, and hardware result as the candidate
advances. Do not create duplicate manifest or QC-status documents.

Run deterministic validation before senior review. One senior review is the
default. A second independent review is required when a candidate touches
protected direct controls, physical Fn/boot/reset behavior, layer structure,
combos, shared/runtime behavior, or three or more physical positions.

User approval of the exact reviewed keymap hash is required before build.
Hardware promotion moves passed candidates to `B1PRO/releases/vx.x/` per root `AGENTS.md` rules.
