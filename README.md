# gbpp

gbpp is a work-in-progress Nintendo Game Boy emulator written in C++.

![](https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExcWpwMWw1N2xjZnM1YTV2OTluOWxyNmh2dWdpZTJseWFyejh5anN6OSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/IkLqRtKNGnOXWWTmDk/source.gif)

It runs many commercial games and passes many popular test programs.

Currently only Windows is supported.

# Roadmap
1. Improve documentation.
2. Implement audio.
3. Implement remaining MBCs (only MBC1 and MBC3 are functional).
4. Debug timer (does not pass Blargg's interrupt test).
5. Implement interactive debugger.
6. Improve quality of life (open recent files, save states, etc.).
7. Linux support.

# Build Instructions
1. Clone this repository.
2. Open gb++.sln in Visual Studio.
3. Ensure SDL2 is properly linked in the project settings.
4. Build and run.

# Dependency Credits
SDL2 used for graphics and audio.

Dear Imgui used for the GUI.
