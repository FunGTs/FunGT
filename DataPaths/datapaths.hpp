#if !defined(_DATA_PATHS_H_)
#define _DATA_PATHS_H_
#include<vector>
#include<string>
struct ModelPaths {
    /* Struct to set the path for Verte shader, fragmen shader and the model path */
    std::string vs_path; //Vertex Shader Path
    std::string fs_path; //Fragment Shader Path
    std::string path;    //Model Path
    std::vector<std::string> data_path;
};

#endif // _DATA_PATHS_H_
