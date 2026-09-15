#include  "Compile_Run.hpp"
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>
#include <string>
#include <jsoncpp/json/json.h>

using namespace NS_COMPILE_AND_RUN_MODULE;


int main()
{
    std::string code;
    FileUtil::ReadFile(PathUtil::Src("code"), &code);

    Json::Value root;
    root["code"] = code;
    root["cpu_limit"] = 1;
    root["mem_limit"] = 50;

    Json::StyledWriter writer;
    std::string in_json = writer.write(root);

    std::string out_json;
    CompileAndRun::Start(in_json, &out_json);

    std::cout << out_json << std::endl;
    return 0;
}
