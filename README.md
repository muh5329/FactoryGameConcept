## Raylib Project


## Build Command

make build
make run


 -> out.exec


## Roadmap

[] Drag movement
[] Pathfinding
[] Physics
[] UI
[] Menu system


## TODO

[] Fix camera movement


## Prerequisites

-- Install Bullet 
[bullet3](https://github.com/bulletphysics/bullet3)

Pull Repo
cd REPO
mkdir -p build && cd build
cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=../install
make -j$(sysctl -n hw.logicalcpu)  # Or just 'make' if unsure
make install

Link Library to CmakeList.txt BULLET_DIR src

-- Install Raylib 
[raylib](https://www.raylib.com/)

 -- On MacOsx --
brew install raylib

## Build Steps

mkdir build
cd build
cmake --build ..
make
