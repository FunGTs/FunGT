#include "path_manager.hpp"

std::string findProjectRoot(){
    std::filesystem::path current = std::filesystem::current_path();
    
    // Traverse up to find the "FunGT" folder
    while (current.has_parent_path()) {
        if (std::filesystem::exists(current / "resources") &&
            std::filesystem::exists(current / "img") &&
            std::filesystem::exists(current / "Animations")) {
            return current.string(); // Found project root
        }
        current = current.parent_path();
    }
    return std::filesystem::current_path().string(); // Fallback
}

std::string getAssetPath(const std::string &path)
{
   return findProjectRoot() + "/" + path;
   
}
