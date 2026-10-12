#pragma once
#include "OJ_Model.hpp"
#include <ctemplate/template_dictionary.h>
#include <ctemplate/template_enums.h>
#include <vector>
#include <ctemplate/template.h>

namespace NS_VIEW_MODULE
{
    using namespace NS_MODEL_MODULE;

    const std::string templatePath = "html_template/";
    const std::string allQuestionsFile = "all_questions.html";
    const std::string oneQuestionFile = "one_question.html";

    class View
    {
    public:
        void AllExpandHtml(std::vector<Question>* questions, std::string* html)
        {
            ctemplate::TemplateDictionary root("root");
            
            for(auto& q : *questions)
            {
                ctemplate::TemplateDictionary* sub_dirt = root.AddSectionDictionary("questions");
                sub_dirt->SetValue("id", std::to_string(q._id));
                sub_dirt->SetValue("title", q._title);
                sub_dirt->SetValue("difficulty", q._diff);
            }

            ctemplate::Template * tpl = ctemplate::Template::GetTemplate(
                    templatePath + allQuestionsFile,
                    ctemplate::DO_NOT_STRIP);
            tpl->Expand(html, &root);
        }

        void OneExpandHtml(Question& q, std::string* html)
        {
            ctemplate::TemplateDictionary root("root");

            root.SetValue("id", std::to_string(q._id));
            root.SetValue("title", q._title);
            root.SetValue("difficulty", q._diff);
            root.SetValue("desc", q._desc);
            root.SetValue("header", q._header);

            ctemplate::Template * tpl = ctemplate::Template::GetTemplate(
                    templatePath + oneQuestionFile,
                    ctemplate::DO_NOT_STRIP);
            tpl->Expand(html, &root);
        }
    };
}
