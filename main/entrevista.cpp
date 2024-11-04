#include<iostream>

int *function(int sizeArray){

    int *output = new int[sizeArray];

    for(int i=0; i<sizeArray; i++){

        output[i] = i;


    }

    return output;

}


int main(){


    int *array{nullptr};
    int sizeArray = 10;

    array = function(sizeArray);

    for(int i=0; i<sizeArray; i++){
        std::cout<<" value = " <<array[i]<<std::endl;
    }

    delete [] array;

    return 0;
}