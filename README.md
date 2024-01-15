# gb++

gb++ is a work-in-progress Nintendo Game Boy emulator written in C++.

![](https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExcWpwMWw1N2xjZnM1YTV2OTluOWxyNmh2dWdpZTJseWFyejh5anN6OSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/IkLqRtKNGnOXWWTmDk/source.gif)

It runs many commercial games and passes many popular test programs.

Currently only Windows is supported.

# Roadmap
1. Implement audio.
2. Implement remaining MBCs (only MBC1 and MBC3 are functional).
3. Debug timer (does not pass Blargg's interrupt test).
4. Implement interactive debugger.
5. Improve quality of life (open recent files, save states, etc.).
6. Linux support.

# Build Instructions
1. Clone this repository.
2. Open gb++.sln in Visual Studio.
3. Ensure SDL2 is properly linked in the project settings.
4. Build and run.

# Dependency Credits
SDL2 used for graphics and audio.

Dear Imgui used for the GUI.
