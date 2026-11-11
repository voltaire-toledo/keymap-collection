# Disabled workflows

Only GitHub Pages publication workflows remain active in `.github/workflows/`.
The QMK build, ZMK build, and scheduled site link checker are archived here;
GitHub Actions does not discover workflows outside `.github/workflows/`.

Build firmware manually using
[the tested container instructions](../../CONTAINER-FIRMWARE-BUILDS.md).
The archived firmware workflows use moving upstream references and are
historical examples, not the maintained build procedure. Do not move them back
without reviewing their inputs and obtaining approval to re-enable CI.

`site-deploy.yml` and `static.yml` remain active for GitHub Pages updates.
The validation step in `site-deploy.yml` remains part of that publishing flow.
