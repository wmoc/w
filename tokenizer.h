#include "tokens.h"
#include "parse_error.h"

#include <cstring>
#include <vector>
#include <cctype>
#include <memory>



class tokenizer{
    // easier to operate on
    char const * remaining;

public:
    std::vector<std::unique_ptr<token::token>> token_list;

public:
    explicit tokenizer(std::string const& str){
        remaining = str.c_str();
        run();
    }

    void run();

private:
    bool is_letter(char c){
        // only standard latin letters
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
    }
    
    bool is_digit(char c){
        // only standard latin letters
        return ('0' <= c && c <= '9');
    }

    std::unique_ptr<token::variable> parse_variable(){
        std::unique_ptr<token::variable> v = std::make_unique<token::variable>("");
        v->content = remaining[0];
        while(is_letter(remaining[1])){
            v->content += remaining[1];
            remaining ++;
        }
        return v;
    }

    std::unique_ptr<token::number> parse_number(){
        std::string num_str;
        num_str += remaining[0];
        while(is_digit(remaining[1]) || remaining[1] == '.'){
            if(remaining[1] == '.' && num_str.find('.') != std::string::npos){
                throw parse_error("probably bad number format");
            }
            num_str += remaining[1];
            remaining ++;
        }
        std::unique_ptr<token::number> n = std::make_unique<token::number>();
        n->content = atof(num_str.c_str());
        return n;
    }
};

void tokenizer::run(){
    while(1){
        char c = remaining[0];

        if(c == '\0'){
            return;
        }else if(isspace(c)){
            // just skip
        }else if(c == '('){
            token_list.emplace_back(std::make_unique<token::par_left>());
        }else if(c == ')'){
            token_list.emplace_back(std::make_unique<token::par_right>());
        }else if(c == '+'){
            token_list.emplace_back(std::make_unique<token::add>());
        }else if(c == '-'){
            token_list.emplace_back(std::make_unique<token::sub>());
        }else if(c == '*'){
            token_list.emplace_back(std::make_unique<token::mul>());
        }else if(c == '/'){
            token_list.emplace_back(std::make_unique<token::div>());
        }else if(c == '='){
            token_list.emplace_back(std::make_unique<token::eq>());
        }else if(!strncmp("log", remaining, 3)){
            token_list.emplace_back(std::make_unique<token::log>());
            remaining  += 2;
        }else if(tokenizer::is_letter(c)){
            token_list.push_back(tokenizer::parse_variable());
        }else if(tokenizer::is_digit(c)){
            token_list.push_back(tokenizer::parse_number());  // .5 is not a valid number, sorry 
        }

        remaining ++;
    }   
}       