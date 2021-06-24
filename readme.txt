minimal opengl project

use SDL2 and glew

#sdl2 install command
sudo apt-get install libsdl2-2.0

#glew install command
sudo apt-get install -y libglew-dev

#############################
##      compilation
#############################

#simple compile cpp
g++ src/main.cpp -o bin/main

#compile with headers in 'include' folder
g++ src/main.cpp -I include -o bin/main

#compile with headers in 'include' folder and libraries
g++ src/main.cpp -I include -o bin/main
