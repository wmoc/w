#include <iostream>

#include "tokenizer.h"
#include "parser.h"


int main(){
    std::string exp;
    std::getline(std::cin, exp);

    try{
        parser p(exp);
        p.parse();
        auto result = p.eval();

        if(p.namely_name != ""){
            std::cout << p.namely_name << " = " << result << std::endl;
        }else{
            std::cout << result << std::endl;
        }
    }catch(parse_error e){
        std::cerr << e.what() << std::endl;
    }   
 
}
