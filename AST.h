#pragma once 
#include <memory>
#include <vector>
#include <cstddef>
#include <cmath>

// equality := expression '=' expression
// expression := math_expr | simple_expr
// math_expr := - expression | expression OP espression 
// simple_expr := (expression) | atomic
// simple := number | namely | log
// log := log(simple)
// op := ...

struct result{
    float a, b;
    // a*x + b;
};


struct equality;

class AST{
    // root is always "=", so "=" is required
public:
    std::unique_ptr<equality> root; 

    AST(tokenizer const& tk);

    using t_it = std::vector<std::unique_ptr<token::token>>::const_iterator;
};

AST::t_it find_type(AST::t_it begin, AST::t_it end, decltype(typeid(token::token)) type){
    int nesting = 0;
    for(auto i = begin; i != end; i++){
        if(typeid(**i) == typeid(token::par_left)){
            nesting ++;
        }else if(typeid(**i) == typeid(token::par_right)){
            nesting --;
        }
        if(typeid(**i) == type && nesting == 0){
            return i;
        }
    }
    return end;
}




struct expression{
    static std::unique_ptr<expression> get_expression(AST::t_it begin, AST::t_it end);

    virtual result eval()=0;
};

struct atomic : expression{
    static std::unique_ptr<atomic> get_atomic(AST::t_it begin, AST::t_it end);
};

struct atomic_number : atomic{
    float content;

    explicit atomic_number(float n){
        content = n;
    }

    atomic_number(AST::t_it begin, AST::t_it end);


    result eval(){
        return {0, content};
    }
};

struct atomic_log : atomic{
    std::unique_ptr<expression> content;

    atomic_log(AST::t_it begin, AST::t_it end);

    result eval(){
        auto r = content->eval();
        if(r.a != 0){
            throw parse_error("Only linear equations available!");
        }

        return {0, (float)log(r.b)};
    }
};

struct atomic_namely : atomic{
    std::string content;

    atomic_namely(AST::t_it begin, AST::t_it end);

    result eval(){
        return {1, 0};
    }
};





struct simple_expr: expression{
    std::unique_ptr<expression> content;

    simple_expr(AST::t_it begin, AST::t_it end){
        if(std::distance(begin, end) > 2 && typeid(**begin) == typeid(token::par_left) && typeid(**(end - 1)) == typeid(token::par_right)){
            content = expression::get_expression(begin+1, end-1);
        }else{
            content = atomic::get_atomic(begin, end);
        }
    }


    result eval(){
        return content->eval();
    }

};


struct math_expr : expression{
    std::unique_ptr<expression> left;
    std::type_info const *op = &typeid(nullptr_t);
    std::unique_ptr<expression> right;

    math_expr(AST::t_it begin, AST::t_it end){
        // -x -> 0-x
        if(begin != end && typeid(**begin) == typeid(token::sub)){
            left = std::make_unique<atomic_number>(0.0f);
            op = &typeid(token::sub);
            right = expression::get_expression(begin + 1, end);
            return; 
        }

        std::vector<std::type_info const *> operators = {&typeid(token::add), &typeid(token::sub), &typeid(token::mul), &typeid(token::div)};
        for(auto const& oprtr : operators ){
            auto pos = find_type(begin, end, *oprtr);
            if(pos != end){
                left = expression::get_expression(begin, pos);
                op = oprtr;
                right = expression::get_expression(pos+1, end);
                return;
            }
        }  
        
        throw parse_error();
    }


    result eval(){
        auto l = left->eval();
        auto r = right->eval();

        if(*op == typeid(token::add))
            return {l.a + r.a, l.b + r.b};
        if(*op == typeid(token::sub))
            return {l.a - r.a, l.b - r.b};
        if(*op == typeid(token::mul)){
            if(l.a != 0 && r.a != 0){
                throw parse_error("Quadratic equation appeared");
            } // (a*x+b)*(c*x+d) = (a*c*x**2 + (a*d+c*b)*x + b*d)
            return {l.a*r.b + r.a*l.b, l.b * r.b};
        }
        if(*op == typeid(token::div)){
            if(l.a != 0 || r.a != 0){
                throw parse_error("Division not supported in linear equation");
            }
            return {l.a / r.a, l.b / r.b};
        }
        
    }
};



struct equality{
    std::unique_ptr<expression> left;
    std::unique_ptr<expression> right;

    equality(AST::t_it begin, AST::t_it end){
        auto pos = find_type(begin, end, typeid(token::eq));

        left = expression::get_expression(begin, pos);
        right = expression::get_expression(pos + 1, end);
    }

    result eval(){
        auto l = left->eval();
        auto r = right->eval();

        return {l.a - r.a, l.b - r.b};
    }
};
    

