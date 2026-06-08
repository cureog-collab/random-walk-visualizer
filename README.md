# Random Walk Engine
A real-time graphical simulation of a 2D Random Walk algorithm, built in C using the SDL2 library. 

This project is for me to familiarize myself with SDL.

## Usage
Compile the source code using:

make

Launch the program via the generated executable. You can use flags to customize the visualizer:

./random_walk [options]

### Flags

-d : Launch in darkmode (the visualizer defaults to lightmode).

-b : Enable blend mode. The agent's path will be rendered with alpha blending, making frequently crossed areas appear darker.

-s: Enable self-avoiding walk (SAW) model. The agent's path won't cross itself.

-g: Enable Gaussian randomwalk model.

-l: Enable Lévy flight model.

### Controls

Click & Drag: Pan the camera around the canvas.

Mouse Wheel: Zoom in and out.

SPACE: Reset the camera viewport back to the origin (0, 0).

S: Pause / Resume the simulation.

## Dependencies

* **SDL2**
* **SDL2_ttf**