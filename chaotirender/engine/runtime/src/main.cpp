#include <iostream>

#include <runtime/render_pipeline.h>

int main(int argc, char** argv) 
{   
    // TODO: 
    // 1. option flag: eraly-z, back face culling
    // 2. shader define variables: atrribute, varying, uniform
    // 3. does renderPipeline need to be global?
    // Chaotirender::testPipeline();
    Chaotirender::runPipeline();

    return 0;
}