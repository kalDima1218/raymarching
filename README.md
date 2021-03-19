# Raymarching
Raymarching is a technology for rendering scenes. It is interesting because the characteristics and positions of objects are described by formulas, which means you can render scenes in any resolution

## Third-party libraries 
For .png files saving, i'm used: https://github.com/lvandeve/lodepng

## Features!

  - Multithreading
  - ASCII arts
  - Image saving in .png
  - Pre-created base figure classes
  - You can create your own figure (fractals too!)
  - You can create fractals
  - Automatic selection of the optimal number of threads (provided in example)
  - DESIGNED AND CODED IN RUSSIA(🇷🇺)
  
## Todos

  - GPU support
  - Write custom .png saving

## Compiling
If you have a your_program.cpp that #includes and uses raymarching.h, you can build as follows:
```sh
g++ your_program.cpp -pthread
```
Example compiling and running (linux):
```sh
g++ -o main main.cpp -pthread; ./main
```

## License
----

LGPL-v2.1

    Raymarching lib. Vector engine library for rendering scenes in high resolution.
                   Copyright © 2020 Dmitry Kalugin
    Raymarching lib is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
    
    Raymarching lib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
    
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA


**If you have any ideas on how to improve the algorithm, write to issues or on my email: kalugin180806@gmail.com**
