# igl_GfCS example project

A blank project example showing how to use igl_GfCS, Eigen and cmake. It shows input/output of mesh data, display and keyboard interaction, as well as drawing overlays. It can be used as a starting point to create a new project using igl_GfCS.

## Download igl_GfCS

The cmake build system will attempt to find igl_GfCS according to environment variables (e.g., `IGL_GfCS`). The first time you try to build the example, you should clone a copy of igl_GfCS inside the directory **above** the example/ directory.

    cd PATH_TO_EXAMPLE/..
    git clone https://github.com/olkido/igl_GfCS.git

## Compile

Compile this project using the standard cmake routine:

    cd PATH_TO_EXAMPLE/
    mkdir build
    cd build
    cmake -DIGL_GfCS="$PWD/../../igl_GfCS" ../
    make

This should find and build the dependencies and create an `example` binary.

### Note for linux users

Many linux distributions do not include gcc and the basic development tools in their default installation. On Ubuntu, you need to install the following packages:

    sudo apt-get install git
    sudo apt-get install build-essential
    sudo apt-get install cmake
    sudo apt-get install libx11-dev
    sudo apt-get install mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev
    sudo apt-get install libxrandr-dev
    sudo apt-get install libxi-dev
    sudo apt-get install libxmu-dev
    sudo apt-get install libblas-dev
    sudo apt-get install libxinerama-dev
    sudo apt-get install libxcursor-dev

## Run

From within the `build` directory just issue:

    ./example PATH_TO_A_MESH_FILE

where PATH_TO_A_MESH_FILE is the path to e.g. an .obj or .off mesh file.
A viewer window should launch displaying a triangle mesh, with  a couple of menu boxes in the window. Hitting '1'/'2' should toggle the display of the bounding box of the mesh.
