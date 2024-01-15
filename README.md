# gb++

gb++ is a work-in-progress Nintendo Game Boy emulator written in C++.

It runs many commercial games and passes many popular community test programs.

Currently only Windows is supported.

# Roadmap
1. Implement audio.
2. Implement remaining MBCs (only MBC1 and MBC3 are functional).
3. Debug timer (does not pass Blargg's interrupt test).
4. Improve quality of life (open recent files, save states, etc.).
5. Linux support.

# Build Instructions
1. Clone this repository.
2. Open gb++.sln in Visual Studio.
3. Ensure SDL2 is properly linked in the project settings.
4. Build and run.

# Dependency Credits
Dear Imgui used for the GUI.

SDL2 used for graphics and audio.
