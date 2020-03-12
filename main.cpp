#include <fstream>
#include "scompulator.h"

int main(){
    std::ifstream infile;
    infile.open("broken.mif");
    Scompulator scomp = Scompulator(infile);
    // scomp.dumpMemory();
    for(int i = 0; i < 50; i++){
        if(scomp.execute()){
            std::cout << "Halted" << std::endl;
            break;
        }
    }
    scomp.dumpMemory();
}