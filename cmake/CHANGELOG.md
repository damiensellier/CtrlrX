
## 2024-08-12

* Bump to Catch2 v3.7.0
* No longer try to build universal binaries on iOS.
* Fix issue with Benchmark globs not pulling in .h files.
* Instead of linking like so `target_link_libraries(Benchmarks PRIVATE SharedCode Catch2::Catch2WithMain)` we now link like so `target_link_libraries(Benchmarks PRIVATE SharedCode Catch2::Catch2)`. This allows us to run JUCE's `ScopedJuceInitialiser_GUI` inside Catch2 for the duration of the tests, see Pamplejuce for the implementation. 

## 2024-07-23

* Bumping VERSION now will reconfigure CMake to pick up new the version

## 2024-06-19

* Static link the Visual C++ Redistributable on Windows

## 2024-06-07

* Bump to Catch2 v3.6.0

## 2024-02-06

* Enforce that targeting macOS down to 10.13 via CMAKE_OSX_DEPLOYMENT_TARGET 

## 2023-09-04

* Added `SharedCodeDefaults.cmake` which handles setting C++20 and fast math on the `SharedCode` Target.
* Modified CTest to report on failure

## 2023-09-04

Initial commit. Added this CHANGELOG.
