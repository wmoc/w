#pragma once
#include "AST.h"
#include "AST.inl"

#include <memory>

class parser{
    std::string exp;
    std::unique_ptr<AST> ast;

public:
    std::string namely_name;

private:
    void obtain_namely_name(tokenizer const& tn);
    void implicit_mul(tokenizer& tn);
    void make_equation(tokenizer& tn);

public: 
    parser(std::string const& str): exp{str}
    {}

    parser(std::string&& str): exp{std::move(str)}
    {}

    void parse(){
        tokenizer tn(exp);

        obtain_namely_name(tn);
        implicit_mul(tn); // 0.5x -> 0.5 * x
        make_equation(tn); // (3+(4-1))*5 -> ((3+(4-1))*5) = result


        ast = std::make_unique<AST>(tn);
    }

    float eval(){
        auto res = ast->root->eval();  // a*x + b = 0
        float ret = -(res.b / res.a);
        if(ret == 0){
            return 0; // getting rid of -0
        }
        return ret;
    }
};


void parser::obtain_namely_name(tokenizer const& tn){
    bool is_equation = false;
    for(auto const& t : tn.token_list){
        if(typeid(*t) == typeid(token::variable)){
            if(namely_name == ""){
                namely_name = static_cast<token::variable *>(t.get())->content;
            }else if(static_cast<token::variable *>(t.get())->content != namely_name){
                throw parse_error("Only one namely supported!");
            }
        }else if(typeid(*t) == typeid(token::eq)){
            is_equation = true;
        }
    }

    if(namely_name == "" && is_equation){
        throw parse_error("There must be namely in equation");
    }else if(namely_name != "" && !is_equation){
        throw parse_error("Namely possible only in equations");
    }
}


void parser::implicit_mul(tokenizer& tn){
    auto end = tn.token_list.end() - 1;
    for(auto i = tn.token_list.begin(); i != end; i++){
        if(typeid(**i) == typeid(token::number) 
            &&( typeid(**(i+1)) == typeid(token::par_left)
            || typeid(**(i+1)) == typeid(token::log)
            || typeid(**(i+1)) == typeid(token::variable))){
            tn.token_list.insert(i+1, std::make_unique<token::mul>());
            i++;
        }
    }
}

void parser::make_equation(tokenizer& tn){
    if(namely_name == ""){
        tn.token_list.emplace_back(std::make_unique<token::eq>());
        tn.token_list.emplace_back(std::make_unique<token::variable>("result"));
    }
}