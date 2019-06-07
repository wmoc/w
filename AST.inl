#pragma once



AST::AST(tokenizer const& tk){
    root = std::make_unique<equality>(tk.token_list.begin(), tk.token_list.end());
}


std::unique_ptr<atomic> atomic::get_atomic(AST::t_it begin, AST::t_it end){
    try{
        return std::make_unique<atomic_log>(atomic_log(begin, end));
    }catch(parse_error){
        try{
            return std::make_unique<atomic_namely>(atomic_namely(begin, end));
        }catch(parse_error){
            return std::make_unique<atomic_number>(atomic_number(begin, end));
        }
    }
}


atomic_number::atomic_number(AST::t_it begin, AST::t_it end){
    if(begin == end - 1 && typeid(**begin) == typeid(token::number)){
        content = (static_cast<token::number *>(begin->get()))->content;
    }else{
        throw parse_error();
    }

}


atomic_log::atomic_log(AST::t_it begin, AST::t_it end){
    if(std::distance(begin, end) > 3 
        && typeid(**begin) == typeid(token::log)
        && typeid(**(begin + 1)) == typeid(token::par_left) 
        && typeid(**(end - 1)) == typeid(token::par_right)){
        content = expression::get_expression(begin+2, end-1);
    }else{
        throw parse_error();
    }

}



atomic_namely::atomic_namely(AST::t_it begin, AST::t_it end){
    if(begin == end - 1 && typeid(**begin) == typeid(token::variable)){
        content = (static_cast<token::variable *>(begin->get()))->content;
    }else{
        throw parse_error();
    }

}


std::unique_ptr<expression> expression::get_expression(AST::t_it begin, AST::t_it end){
    try{
        return std::make_unique<simple_expr>(begin, end);
    }catch(parse_error){
        return std::make_unique<math_expr>(begin, end);
    }
}