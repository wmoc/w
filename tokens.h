#pragma once

#include <string>
#include <typeinfo>

namespace token{
    struct token{
        virtual operator std::string() =0;
        // virtual std::string name(){ return std::string(*this); }
    };
    struct par_left : public token{
        operator std::string(){ return "("; }
    };
    struct par_right : public token{
        operator std::string(){ return ")"; }
    };
    struct add : public token{
        operator std::string(){ return "+"; }
    };
    struct sub : public token{
        operator std::string(){ return "-"; }
    };
    struct mul : public token{
        operator std::string(){ return "*"; }
    };
    struct div : public token{
        operator std::string(){ return "/"; }
    };
    struct eq : public token{
        operator std::string(){ return "="; }
    };
    struct log : public token{
        operator std::string(){ return "log"; }
    };
    struct variable : public token{
        std::string content;

        variable(std::string const& str) : content(str){}

        operator std::string(){
            return std::string() + "variable(" + content + ")";
        }
    };
    struct number : public token{
        float content;
        
        operator std::string(){
            return std::string() + "number(" + std::to_string(content) + ")";
        }
    };
};