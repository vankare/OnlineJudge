#include  "Compile_Run.hpp"
#include "../Comm/httplib.h"
#include <string>

using namespace NS_COMPILE_AND_RUN_MODULE;
using namespace httplib;

void Usage(std::string proc)
{
    std::cerr << "Usage:\n\t" << proc << "\tprot\n";
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        return 1;
    }

    Server svr;

    svr.Post("/compile_and_run", 
        [](const Request& req, Response& rep)
        {
            std::string in_json = req.body;
            std::string out_json;
            if(!in_json.empty())
            {
                CompileAndRun::Start(in_json, &out_json);
                rep.set_content(out_json, "application/json;charset=utf-8");
            }
        });

    svr.listen("0.0.0.0", std::stoi(argv[1]));

    return 0;
}
