#include "sliceAlgo/SliceAlgo.h"
#include "stl/loadSTL.h"
#include <iostream>
#include <iomanip>
#include "geometry/Geometry.h"
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    // Check if the user provided exactly 1 argument (besides the program name)
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <directory_path>" << std::endl;
        return 1;  // Return error code
    }

    // argv[1] contains the directory path
    std::string directoryPath = argv[1];

        // Convert the input path to a filesystem path object
    fs::path inputFsPath(directoryPath);

    // Get the parent directory path
    fs::path parentPath = inputFsPath.parent_path();

    std::cout << "Input path: " << inputFsPath << std::endl;
    std::cout << "Parent path: " << parentPath << std::endl;

    std::vector<Geometry::Triangle> tris = stl::loadBinarySTL(directoryPath);
    Geometry::translateTriangulation(tris, 35, 30);
    // 4d transformation into coordinate system?
    // scale complete 3d model instead of images?
    //  get bounding box of 3d model each to calc height?
    float Height = 10.f;
    SlicerAlgo algo(Height, 0.1f, tris, parentPath.string());
    bool ready = algo.run();
    return 0;
}