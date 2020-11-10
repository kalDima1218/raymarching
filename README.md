# Raymarching
Raymarching is a technology for rendering scenes. It is interesting because the characteristics and positions of objects are described by formulas, which means you can render scenes in any resolution

# ATTENTION!!! 
For .png files saving, i'm used: https://github.com/lvandeve/lodepng, so I INCLUDED IN IN MY LIBRARY! If you using it too, you may not include it

# Features!

  - Multithreading
  - ASCII arts
  - Image saving in .png
  - Pre-created base figure classes
  - You can create your ouw figure (fractales too!)
  - You can create fractales
  - Automatic selection of the optimal number of threads

### Compiling
If you have a hypothetical your_program.cpp that #includes and uses raymarching.h, you can build as follows:
```sh
g++ your_program.cpp lodepng.cpp -pthread
```
Example compiling for linux:
```sh
g++ -o main example.cpp lodepng.cpp -pthread//to run it on linux: ./main
```

### Todos

 - Write MORE Tests
 - Add Night Mode

License
----

LGPL


**If you have any ideas on how to improve the algorithm, write to issues**
**Free Software, Hell Yeah!**
