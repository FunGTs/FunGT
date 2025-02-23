#if !defined(_PATH_MANAGER_H_)
#define _PATH_MANAGER_H_
#include <string>
#include <iostream>
#include <filesystem>
std::string findProjectRoot();
std::string getAssetPath(const std::string &path);




#endif // _PATH_MANAGER_H_
