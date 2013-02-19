#include "Framework/FrameworkAPI.hpp"

#include <iostream>
#include <string>

int main()
{

    std::string sGdfPath = "Test.gdf";
    std::clog << "Initializing Smoke" << std::endl;
    EngineExecuteGDF(sGdfPath.c_str());
    std::clog << "Exiting Smoke" << std::endl;  
    
    return 0;
}
