#include <iostream>
#include "../Comm/httplib.h"
#include "OJ_Control.hpp"
using namespace httplib;
using namespace NS_CONTROL_MODULE;


int main()
{
    Server svr;
    Control ctrl;

    svr.set_base_dir("./wwwroot");

    svr.Get("/all_questions",
        [&ctrl](const Request& req, Response& resp)
        {
            std::string html;
            ctrl.AllQuestions(&html);
            resp.set_content(html, "text/html; charset=utf-8");
        });

    svr.Get(R"(/question/(\d+))",
        [&ctrl](const Request& req, Response& resp)
        {
            std::string number = req.matches[1];
            std::string html;
            ctrl.OneQuestion(std::stoi(number), &html);

            resp.set_content(html, "text/html; charset=utf-8");
        });

    svr.Post(R"(/judge/(\d+))",
        [&ctrl](const Request& req, Response& resp)
        {
            std::string number = req.matches[1];

            std::string in_json = req.body;
            std::string out_json;
            ctrl.Judge(std::stoi(number), in_json, &out_json);

            resp.set_content(out_json, "text/html; charset=utf-8");
        });

    svr.listen("0.0.0.0", 3999);
    return 0;
}
