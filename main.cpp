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
#include <map>

#include <signal.h>
#include <readline/readline.h>

static bool running = false;
static int program_size;

static void run(processor & p){
    
    p.pc = 0;
    p.sp = std::numeric_limits<processor::word>::max();
    running = true;
    
    while(running){
        p.run();
    }
    
    std::cout<<std::endl;
}

static void dump_mem(processor & p){
    
    for(processor::word i = 0; i < program_size; i++){
        std::cout<<std::setw(4)<<std::setfill('0')<<std::hex<<p.ram[i]<<" ";
        if((i + 1) % 8 == 0) std::cout<<std::endl;
    }
    
    std::cout<<std::endl;
}

static void dump_registers(processor & p){
    
    const char * general_registers[] = {
        "A", "B", "C", "X", "Y", "Z", "I", "J"  
    };
    
    std::cout.setf(std::ios::hex, std::ios::basefield);
    
    std::cout<<"PC:"<<std::setw(4)<<std::setfill('0')<<p.pc<<" ";
    std::cout<<"SP:"<<std::setw(4)<<std::setfill('0')<<p.sp<<std::endl;
    
    for(unsigned int i = 0; i < processor::REGN; i++){
        std::cout<<general_registers[i]<<": "<<std::setw(4)<<std::setfill('0')<<p.reg[i]<<((i + 1) % 3 == 0 ? "\n" : " ");
    }
    std::cout<<std::endl;
    
    std::cout<<"O: "<<std::setw(4)<<std::setfill('0')<<p.overflow<<std::endl;
}

static void quit(processor & p){
    exit(0);
}

static void interrupt(int){
    
    if(running == false){
        exit(0);
    }
    
    running = false;
}

int main(int argc, const char ** argv){
    
    signal(SIGINT, interrupt);
    
    std::map<std::string, void(*)(processor &)> ui_commands;
    ui_commands["quit"] = quit;
    ui_commands["mem"] = dump_mem;
    ui_commands["regs"] = dump_registers;
    ui_commands["run"] = run;
    
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
        
        program_size = generate_code(parse_result, symbols, ram);
        
        processor processor;
        processor.ram = ram;
        
        while(std::cin.good()){
            
            const char * line = readline("> ");
            
            if(!line) break;
            if(line[0] == '\0') continue;
            
            auto ui_command_iter = ui_commands.find(line);
            
            if(ui_command_iter != ui_commands.end()){
                (*ui_command_iter).second(processor);
            }
            else{
                std::cout<<"unknown command"<<std::endl;
            }
        }
    }
    catch(const parse_error & error){
        std::cerr<<filename<<":"<<error.where().line()<<":"<<error.where().column()<<" Parse error: "<<error.what()<<std::endl;
    }
    catch(const symbol_error & error){
        std::cerr<<filename<<":"<<error.where().line()<<":"<<error.where().column()<<" Symbol error: "<<error.what()<<std::endl;
    }
    catch(const type_error & error){
        std::cerr<<filename<<":"<<error.where().line()<<":"<<error.where().column()<<" Type error: "<<error.what()<<std::endl;
    }
    catch(const processor::exception & error){
        std::cerr<<error.what()<<std::endl;
    }
    
    return 0;
}
