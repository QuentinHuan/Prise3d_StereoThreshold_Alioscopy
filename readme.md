Prise3D stereo thresholds experiment : works on alioscopy autostereoscopic screen
This program was made to mesure perceptive noise thresholds for MonteCarlo pathtraced images.

uses: OpenGL, SDL2, glad,
additional : lodepng (in /include), glm math lib

# Dependencies:

*SDL2 install command:
```sudo apt-get install libsdl2-2.0``` and ```sudo apt install libsdl2-dev```

*GLM Math lib install command:
``sudo apt install libglm-dev``

*SDL2-TTF install command:
``sudo apt-get install libsdl2-ttf-dev``

*lib Freetype install command:
``sudo apt-get install libfreetype6-dev``

# Python scripts setup:
Scripts are in ```./Prise3d_StereoThreshold_DataAnalysis```. Check [here](https://github.com/QuentinHuan/Prise3d_StereoThreshold_DataAnalysis) for detailed instructions

# Compile and run:
``mkdir build 
cd build
cmake ..
make ``

Then ``./build/main`` to run

# Command line arguments:
*``--demo`` to run the program in demo mode (program will run using ``config/sceneDemo.ini`` and ``config/configDemo.ini``)
