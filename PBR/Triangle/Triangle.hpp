#if !defined(_TRIANGLE_H_)
#define _TRIANGLE_H_
#include "../../Vector/vector3.hpp"
#include "../../Material/material.hpp"

class Triangle{


    public: 
        fungt::Vec3 v0, v1, v2; 
        fungt::Vec3 normal;
        Material matrial;
        
        Triangle(){
            
        }
        ~Triangle(){

        }





};




#endif // _TRIANGLE_H_
