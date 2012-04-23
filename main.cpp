#include "processor.hpp"
#include "asm/parser.hpp"
#include "asm/symbol_analyser.hpp"
#include "asm/symbol_error.hpp"
#include "asm/type_error.hpp"
#include "asm/code_generator.hpp"

#include <string>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

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
        auto parse_result = parse(begin, end);
        
        symbol_table symbols;
        load_predefined_symbols(symbols);
        load_symbols(parse_result, symbols);
        
        processor::word ram[128000];
        
        processor::word program_size = generate_code(parse_result, symbols, ram);
        
        processor processor;
        processor.ram = ram;
        processor.pc = 0;
        processor.sp = std::numeric_limits<processor::word>::max();
        
        while(std::cin.good()){
            processor.run();
        }
        
        for(processor::word i = 0; i < program_size; i++){
            std::cout<<std::setw(4)<<std::setfill('0')<<std::hex<<ram[i]<<" ";
            if((i + 1) % 8 == 0) std::cout<<std::endl;
        }
        
        std::cout<<std::endl;
    }
    catch(const parse_error & error){
        std::cout<<filename<<":"<<error.where().line()<<":"<<error.where().column()<<" Parse error: "<<error.what()<<std::endl;
    }
    catch(const symbol_error & error){
        std::cout<<filename<<":"<<error.where().line()<<":"<<error.where().column()<<" Symbol error: "<<error.what()<<std::endl;
    }
    catch(const type_error & error){
        std::cout<<filename<<":"<<error.where().line()<<":"<<error.where().column()<<" Type error: "<<error.what()<<std::endl;
    }
    catch(const processor::exception & error){
        std::cout<<error.what()<<std::endl;
    }
    
    return 0;
}
