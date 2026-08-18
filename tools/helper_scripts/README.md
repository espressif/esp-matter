# Helper Scripts

Utility scripts for esp-matter development and maintenance.

| Script | Description |
|--------|-------------|
| [submodule_or_idf_bump_pre_checks.sh](submodule_or_idf_bump_pre_checks.sh) | Pre-checks before a connectedhomeip submodule or ESP-IDF version bump: regenerates the data model, runs its unit tests, formats generated files, and regenerates zap_common and cluster_select files. Run from the esp-matter root. Requires Python 3.12. |
