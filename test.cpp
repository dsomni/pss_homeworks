#include "WendexTaxi.cpp"

using namespace std;

/********************************************************
*                       Testing                         *
********************************************************/

int main(){

    WendexTaxi WT;
    WT.init();

    // You can change different types of testing
    // It is better to start each method several times
    //
    // Description of each function is in WendexTaxi.cpp
    //
    // Actually, you can start both, but it is going to be messy

    WT.startSimulationNewUsers();
    //WT.startSimulationExistingUsers();

    return 0;
}
