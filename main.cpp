#include <iostream>
#include "layout.hpp"
#include "io.hpp"
#include "timer.hpp"
#include "router.hpp"
int main(int argc, char const *argv[]){
    Timer timer;

    Layout layout;

    std::cout << "Reading Layout\n"; timer.setShortTerm();
    io::readLayout(&layout, argv[1]);
    std::cout << "Read time: " << timer.getShortTerm() << "\n";
    
    std::cout << "Routing\n"; timer.setShortTerm();
    Router router(&layout);
    router.main();
    std::cout << "Reroute number: " << router.num_of_reroute << "\n";
    router.countNumOfViolation();
    std::cout << "Violation number: " << router.num_of_violation << "\n";
    std::cout << "Routing time: " << timer.getShortTerm() << "\n";
    
    std::cout << "Write Layout\n"; timer.setShortTerm();
    io::writeLayout(&layout, argv[2]);
    std::cout << "Write time: " << timer.getShortTerm() << "\n";
    
    std::cout << "Total time: " << timer.getAndReset() << "\n";
    return 0;
}