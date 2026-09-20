# Changelog

All notable changes to this project will be documented in this file.

## September 20, 2026 - Version 0.1.2
### Changed
- No longer display a past event on countdown start up
- Set initial event to next earliest target date from the list of events
- If target event is today, set it as initial countdown event on power up

## September 2, 2026 - Version 0.1.1
### Fixed
- Bug fix: accumulated time drift caused countdown display to skip seconds
- Replace delay in main loop with RTC-second check

## August 11, 2026 - Version 0.1.0
### Added
- Initial version
- README.md
- Arduino sketch for coutdown timer