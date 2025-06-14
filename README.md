## Raylib Project


## Build Command

make build
make run


 -> out.exec


## Roadmap

[X] Drag movement
[] Pathfinding
[X] Physics
[] UI
[] Menu system


## Musings
[Thoughts](Thoughts.md)

## Blog 
[Precursor](blog/001.md)
[ReCast](blog/002.md)
[A* Pathfinding](blog/003.md)
[Pathfinding optimization](blog/004.md)

## Docs 
[a* Algorithm](src/Common/pathfinding/a_*.MD)

## TODO

[] Fix camera movement
[] Swap between FPS and RTS Camera
[] recast for pathfinding [recast](https://github.com/recastnavigation/recastnavigation/blob/main/Docs/_99_Roadmap.md)


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

## Change Makefile and CMakeFile Lib Paths
 - Change set(BULLET_DIR "/Users/muneebhaq/Documents/projects/c_lib/bullet3/install/")
### MakeFile
- Change core path /Users/muneebhaq/Documents/projects/FactoryGameConcept

## Build Steps

mkdir build
cd build
cmake ..
cmake --build .
make
./FactoryGameConcept 