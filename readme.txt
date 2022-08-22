Jani Järvenpää 2022

This project acts as a starting point for c++ based OpenGL game. 
I have no idea how to make a game, but i can setup build system.

This setup uses VScode with C/C++ extension as a debugger. 
You need WSL2 and Remote - WSL extension for VScode.


windows 10 xserver setup:
- install vcxsrv https://sourceforge.net/projects/vcxsrv/files/latest/download
- run xlaunch > Multiple windows > start no client > tick all the boxes,
- (OPTIONAL) make sure firewall does not block the connection: Control Panel > System and Security > Windows Defender Firewall > Advanced Settings > Inbound Rules > New Rule...
> Program > %ProgramFiles%\VcXsrv\vcxsrv.exe > Allow the connection > checked Domain/Private/Public > Named and Confirmed Rule.

WSL setup, Run following commands to setup build tools and libraries:
   sudo apt update && sudo apt install build-essential gdb -y 
    sudo apt-get install cmake pkg-config -y
    sudo apt-get install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev -y
    sudo apt-get install libglew-dev libglfw3-dev libglm-dev -y 
    sudo apt-get install libao-dev libmpg123-dev libxinerama-dev libxcursor-dev libxi-dev -y
    cd /usr/local/lib/
    sudo git clone https://github.com/glfw/glfw.git
    cd glfw
    sudo cmake .
    sudo make
    sudo make install
    cd ~
Run these to setup and test xserver window
    export DISPLAY="$(grep nameserver /etc/resolv.conf | sed 's/nameserver //'):0"
    sudo apt install x11-apps -y
    xeyes
if you see window with eyes, setup was successful. 



OpenGL examples:
https://cs.lmu.edu/~ray/notes/openglexample