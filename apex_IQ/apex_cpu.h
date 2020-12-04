/*
 * apex_cpu.h
 * Contains APEX cpu pipeline declarations
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#ifndef _APEX_CPU_H_
#define _APEX_CPU_H_

#include "apex_macros.h"
#include <stdbool.h>

/* Format of an APEX instruction  */
typedef struct APEX_Instruction
{
    char opcode_str[128];
    int opcode;
    int rd;
    int rs1;
    int rs2;
    int rs3;
    int imm;
} APEX_Instruction;

/* Model of CPU stage latch */
typedef struct CPU_Stage
{
    int pc;
    char opcode_str[128];
    int opcode;
    int rs1;
    int rs2;
    int rs3;
    int rd;
    int imm;
    int rs1_value;
    int rs2_value;
    int rs3_value;
    int result_buffer;
    int memory_address;
    int has_insn;
} CPU_Stage;

/* Model of APEX CPU */
typedef struct APEX_CPU
{
    int pc;                        /* Current program counter */
    int clock;                     /* Clock cycles elapsed */
    int insn_completed;            /* Instructions retired */
    int regs[REG_FILE_SIZE];       /* Integer register file */
    int code_memory_size;          /* Number of instruction in the input file */
    APEX_Instruction *code_memory; /* Code Memory */
    int data_memory[DATA_MEMORY_SIZE]; /* Data Memory */
    int single_step;               /* Wait for user input after every cycle */
    int zero_flag;                 /* {TRUE, FALSE} Used by BZ and BNZ to branch */
    int fetch_from_next_cycle;

    /* Pipeline stages */
    CPU_Stage fetch;
    CPU_Stage decode;
   CPU_Stage execute_INTU;
    CPU_Stage execute_MULU;
    CPU_Stage execute_MEMU;
    CPU_Stage memory;
    CPU_Stage writeback;


    int ROB_HEAD;
    int ROB_TAIL;
    struct ROB_ENTRY
        {
        int pc;
        int ar;
        int result;
        int result_valid;
        int excodes;
        int fu_type;
        int pr;
        int halt;
        int free;

        }ROB[64];
    // status bit for dependency
    //char valid_reg_bit[16];// if 0 then no depedency and no stall, 1 means dependency and stall
    // Issue Q
    struct IQ_ENTRY
    {
    int rob_index;
    int prd;
    int opcode;
    char opcode_str[128];
    int pc;
    int status_bit_IQ;
    int fu_type;
    int imm;
    int freebit;

    int rs1_ready;
    int rs1_value;
    int rs1_tag;

    int rs2_ready;
    int rs2_value;
    int rs2_tag;

    
    int rs3_ready;
    int rs3_value;
    int rs3_tag;

    int memory_address;

    
    }IQ_ENTRY[24];
    // stall flag
    bool stall_flag;
} APEX_CPU;



typedef struct VALID_REG
{
    int reg_key;
    bool valid_bit;
} VALID_REG;


APEX_Instruction *create_code_memory(const char *filename, int *size);
APEX_CPU *APEX_cpu_init(const char *filename,const char *cmd);
void APEX_cpu_run(APEX_CPU *cpu, const int args_index);
void APEX_cpu_stop(APEX_CPU *cpu);
#endif
