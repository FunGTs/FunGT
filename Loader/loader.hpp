#if !defined(_LOADER_H_)
#define _LOADER_H_
#include<functional>
#include <tbb/parallel_for_each.h>
#include <tbb/task_group.h>
#include "../SimpleModel/simple_model.hpp"

class ModelLoader{

    std::vector<std::function<void()>> tasks;

public:
    template<typename ModelType>
    void enqueue(std::shared_ptr<ModelType> model,
        const ModelPaths & path,
        std::function<void(std::shared_ptr<ModelType>)> callback) {

        // store a type-erased lambda in the tasks vector
        tasks.push_back([=]() {
            model->load(path);      // CPU-heavy load
            callback(model);        // safe to call directly here
        });


    }

    void waitForAll() {
        tbb::parallel_for_each(tasks.begin(), tasks.end(), [](auto& task) {
            task();
        });
        tasks.clear();
    }
    
};

#endif // _LOADER_H_
