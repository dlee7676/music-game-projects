Music game demo
by David Lee

This program is a demonstration of a basic music game engine made with DirectX.  It is based on games like Stepmania or Guitar Hero, in which scrolling notes arranged in time with a piece of music have to be hit when they are within a certain range of a target area.  The controls during play are the D, F, J, and K keys, which trigger the target columns from left to right.  

Song clips are from "REM III" by Dimrain47 and "Destination Unknown" by Renard; I do not own them and they are only being used to demonstrate the program.

Video of the program running: http://www.youtube.com/watch?v=1_GLj39t5v0

To compile the program in Visual Studio, add the headers and .cpp files to a blank Win32 project and add the Include folder from the Microsoft DirectX SDK to the Visual C++ include directories.  It is also necessary to add the d3d9.lib and d3dx9.lib files from the Lib/x86 folder in the SDK to the additional dependencies in the linker input options, along with strmiids.lib.  To run the program, the included image and audio files need to be added to the project directory as well.