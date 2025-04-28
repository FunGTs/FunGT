#if !defined(_PARTICLE_SYSTEM_H)
#define _PARTICLE_SYSTEM_H
#include "sycl_handler.hpp"
#include <vector>
#include <functional>
#include <cmath>
#include "particle_set.hpp"

namespace flib{
   
    template <typename T, typename Func>
    class ParticleSystem
    {
        class sycl_handler; // Forward declaration of sycl_handler
        
    public:
        void static update(ParticleSet<T> &particles, Func update_function, T dt){

            std::size_t n = particles._particles.size();
            std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
            std::size_t ydim = xdim;    
            sycl::queue Q = flib::sycl_handler::get_queue();
            {
               
                sycl::buffer<Particle<T>> buf(particles._particles.data(), sycl::range<1>(n));
                Q.submit([&](sycl::handler &cgh) {
                auto acc = buf.template get_access<sycl::access::mode::read_write>(cgh);
                    cgh.parallel_for(sycl::range<2>(sycl::range<2> {static_cast<size_t>(xdim),static_cast<size_t>(ydim)}),[=](sycl::item<2> item) {
                    //user defined lambda function
                    std::size_t index = item[0] * ydim + item[1];
                    if (index < n) {
                        update_function(acc[index], dt);
                    }
                    });
                 });

            }

        }
    
    };
    
}

#endif // _PARTICLE_SYSTEM_H
