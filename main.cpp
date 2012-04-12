#include "processor.hpp"
#include "asm/parser.hpp"

#include <string>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

class test_visitor:public parse_tree_node_visitor{
public:
    
    virtual void visit(const label *){
        std::cout<<"label"<<std::endl;            
    }
    
    virtual void visit(const instruction * ins){
        
        std::cout<<std::string(ins->operation.begin, ins->operation.end)<<" ";
        
        for(int i = 0; i < ins->operands.size(); i++){
            ins->operands[i]->accept(*this);
        }
        std::cout<<std::endl;
    }
    
    virtual void visit(const symbol * s){
        std::cout<<std::string(s->value.begin, s->value.end)<<" ";
    }
     
    virtual void visit(const number *){
        std::cout<<"number"<<std::endl;
    }
    
    virtual void visit(const subscript_symbol *){
        std::cout<<"subscript_symbol"<<std::endl;
    }
    
    virtual void visit(const subscript_number *){
        std::cout<<"subscript_number"<<std::endl;
    }
    
    virtual void visit(const subscript_addition *){
        std::cout<<"subscript_addition"<<std::endl;
    }
    
};

int main(int argc, const char ** argv){
    
    if(argc < 2){
        std::cerr<<"missing filename argument"<<std::endl;
        return 0;
    }
    
    const char * filename = argv[1];
    
    FILE * source_file = fopen(filename, "r");
    
    if(!source_file){
        std::cerr<<"could not open source file"<<std::endl;
        return 0;
    }
    
    std::string source;
    
    while(!feof(source_file)){
        char buffer[4096];
        size_t bytes_read = fread(buffer, 1, sizeof(buffer), source_file);
        if(bytes_read){
            source.append(buffer, bytes_read);   
        }
    }
    
    const char * begin = source.c_str();
    const char * end = begin + source.size();
    
    try{
        const parse_tree_node * node = parse(begin, end);
        
        test_visitor print;
        
        while(node){
            node->accept(print);
            node = node->get_next();
        }
    }
    catch(const parse_error & error){
        std::cout<<filename<<":"<<error.where().line<<":"<<error.where().column()<<" Parse error: "<<error.what()<<std::endl;
    }
    
    return 0;
}
