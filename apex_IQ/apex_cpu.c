/*
 * apex_cpu.c
 * Contains APEX cpu pipeline implementation
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apex_cpu.h"
#include "apex_macros.h"
#include <stdbool.h>

/* Converts the PC(4000 series) into array index for code memory
 *
 * Note: You are not supposed to edit this function
 */

struct VALID_REG arr_reg[16];
int MULU_counter = 0;
bool MULU_Occupied = FALSE;

static int
get_code_memory_index_from_pc(const int pc)
{
    return (pc - 4000) / 4;
}

static void
print_instruction(const CPU_Stage *stage)
{
    switch (stage->opcode)
    {
        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_MUL:
        case OPCODE_DIV:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_LDR:
        case OPCODE_XOR:
        {
            printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rd, stage->rs1,
                   stage->rs2);
            break;
        }

        case OPCODE_MOVC:
        {
            printf("%s,R%d,#%d ", stage->opcode_str, stage->rd, stage->imm);
            break;
        }

        case OPCODE_ADDL:
        case OPCODE_SUBL:
        case OPCODE_LOAD:
        {
            printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
                   stage->imm);
            break;
        }

        case OPCODE_STORE:
        {
            printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs1, stage->rs2,
                   stage->imm);
            break;
        }

        case OPCODE_BZ:
        case OPCODE_BNZ:
        {
            printf("%s,#%d ", stage->opcode_str, stage->imm);
            break;
        }

        case OPCODE_HALT:
        {
            printf("%s", stage->opcode_str);
            break;
        }
        
        case OPCODE_CMP:{
              printf("%s,R%d,R%d", stage->opcode_str, stage->rs1, stage->rs2);
              break;
        }
        case OPCODE_STR:{
              printf("%s,R%d,R%d,R%d", stage->opcode_str, stage->rs1, stage->rs2, stage->rs3);
              break;
        }
    }
}

/* Debug function which prints the CPU stage content
 *
 * Note: You can edit this function to print in more detail
 */
static void
print_stage_content(const char *name, const CPU_Stage *stage)
{
    printf("%-15s: pc(%d) ", name, stage->pc);
    print_instruction(stage);
    printf("\n");
}

/* Debug function which prints the register file
 *
 * Note: You are not supposed to edit this function
 */
static void
print_reg_file(const APEX_CPU *cpu)
{
    int i;

    printf("----------\n%s\n----------\n", "Registers:");

    for (int i = 0; i < REG_FILE_SIZE / 2; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i]);
    }

    printf("\n");

    for (i = (REG_FILE_SIZE / 2); i < REG_FILE_SIZE; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i]);
    }

    printf("\n");
}


static void APEX_RetireINS(APEX_CPU *cpu, int result)
{
    printf("\n In retire INS 1\n");
    if(!cpu->ROB[cpu->ROB_HEAD].halt)
    {
        printf("\n In retire INS 2\n");
        while(cpu->ROB
        [cpu->ROB_HEAD].result_valid == 1 && cpu->ROB_HEAD < cpu->ROB_TAIL && cpu->ROB[cpu->ROB_HEAD].free == 1)
        {
            printf("\n In retire INS 3\n"); 
            cpu->regs[cpu->ROB_HEAD]=result;
            cpu->ROB_HEAD=cpu->ROB_HEAD+1;
            cpu->ROB[cpu->ROB_HEAD].free=0;
            cpu->stall_flag = FALSE;   
        }
    }
    else
    {
        printf("\n End of Code piece\n");
    }
}

static void Issue_Queue(APEX_CPU *cpu){

    int i;
    bool intUApplied=false;
    bool mulUApplied=false;
    bool memUApplied=false;

    

    for(i=0;i<24;i++)
    {   
        
        if(cpu->IQ_ENTRY[i].freebit==1)
        {
            if(cpu->IQ_ENTRY[i].fu_type == 2 && MULU_Occupied==false && mulUApplied==false)
            {

                struct CPU_Stage s1;
                cpu->execute_MULU = cpu->decode;

                s1.pc=cpu->IQ_ENTRY[i].pc;
                memcpy(s1.opcode_str, cpu->IQ_ENTRY[i].opcode_str, sizeof s1.opcode_str);
                s1.opcode= cpu->IQ_ENTRY[i].opcode;
                s1.rs1=cpu->IQ_ENTRY[i].rs1_value;
                s1.rs2=cpu->IQ_ENTRY[i].rs2_value;
                s1.rs3=cpu->IQ_ENTRY[i].rs3_value;
                s1.rd=cpu->ROB[cpu->IQ_ENTRY[i].rob_index].ar;
                s1.imm=cpu->IQ_ENTRY[i].imm;
                s1.rs1_value=cpu->IQ_ENTRY[i].rs1_value;
                s1.rs2_value=cpu->IQ_ENTRY[i].rs2_value;
                s1.rs3_value=cpu->IQ_ENTRY[i].rs3_value;
                s1.memory_address=cpu->IQ_ENTRY[i].memory_address;


                printf("\nIssuing instruction here 1\n");
                    mulUApplied=true;
                  cpu->decode.has_insn = FALSE;
                  

            }
             if(cpu->IQ_ENTRY[i].fu_type == 3 && memUApplied==false)
            {

                struct CPU_Stage s1;
                cpu->execute_MULU = cpu->decode;

                s1.pc=cpu->IQ_ENTRY[i].pc;
                memcpy(s1.opcode_str, cpu->IQ_ENTRY[i].opcode_str, sizeof s1.opcode_str);
                s1.opcode= cpu->IQ_ENTRY[i].opcode;
                s1.rs1=cpu->IQ_ENTRY[i].rs1_value;
                s1.rs2=cpu->IQ_ENTRY[i].rs2_value;
                s1.rs3=cpu->IQ_ENTRY[i].rs3_value;
                s1.rd=cpu->ROB[cpu->IQ_ENTRY[i].rob_index].ar;
                s1.imm=cpu->IQ_ENTRY[i].imm;
                s1.rs1_value=cpu->IQ_ENTRY[i].rs1_value;
                s1.rs2_value=cpu->IQ_ENTRY[i].rs2_value;
                s1.rs3_value=cpu->IQ_ENTRY[i].rs3_value;
                s1.memory_address=cpu->IQ_ENTRY[i].memory_address;


                printf("\nIssuing instruction here 2\n");
                cpu->execute_MEMU = cpu->decode;
                intUApplied=true;
                  cpu->decode.has_insn = FALSE;
                  

            }
             if(cpu->IQ_ENTRY[i].fu_type == 1 && memUApplied==false)
            {
                printf("\n CPU IQ_ENTRY[%d].freebit=%d\n",i,cpu->IQ_ENTRY[i].freebit);
            printf("\n CPU IQ_ENTRY[%d].fu_type=%d\n",i,cpu->IQ_ENTRY[i].fu_type);
                struct CPU_Stage s1;
                cpu->execute_MULU = cpu->decode;

                s1.pc=cpu->IQ_ENTRY[i].pc;
                memcpy(s1.opcode_str, cpu->IQ_ENTRY[i].opcode_str, sizeof s1.opcode_str);
                s1.opcode= cpu->IQ_ENTRY[i].opcode;
                s1.rs1=cpu->IQ_ENTRY[i].rs1_value;
                s1.rs2=cpu->IQ_ENTRY[i].rs2_value;
                s1.rs3=cpu->IQ_ENTRY[i].rs3_value;
                s1.rd=cpu->ROB[cpu->IQ_ENTRY[i].rob_index].ar;
                s1.imm=cpu->IQ_ENTRY[i].imm;
                s1.rs1_value=cpu->IQ_ENTRY[i].rs1_value;
                s1.rs2_value=cpu->IQ_ENTRY[i].rs2_value;
                s1.rs3_value=cpu->IQ_ENTRY[i].rs3_value;
                s1.memory_address=cpu->IQ_ENTRY[i].memory_address;


                printf("\nIssuing instruction here 3\n");
                
                cpu->execute_INTU = s1;
                memUApplied = true;
                  cpu->decode.has_insn = FALSE;
                  

            }

            
        }

        if(intUApplied && mulUApplied && memUApplied)
        {

            break;

        }

    }

    
}

int checkIfRegIsValid(int reg,APEX_CPU *cpu)
{   
   if(arr_reg[reg].reg_key==reg && arr_reg[reg].valid_bit==true)
   {
       return 1;

   }
   else
   {
       return 0;
   }
   

}

static void Populate_Issue_Queue(APEX_CPU *cpu){
    int i;
    for(i=0;i<24;i++)
    {   
        //0 if free
        //printf("\n ith=%d freebit=%d\n",i,cpu->IQ_ENTRY[i].freebit);
        if(cpu->IQ_ENTRY[i].freebit==0)
        {
            cpu->IQ_ENTRY[i].freebit=1;
            cpu->IQ_ENTRY[i].pc=cpu->decode.pc;
            cpu->IQ_ENTRY[i].rob_index = cpu->ROB_TAIL;
            cpu->IQ_ENTRY[i].prd = 0;
            cpu->IQ_ENTRY[i].opcode = cpu->decode.opcode;
            memcpy(cpu->IQ_ENTRY[i].opcode_str, cpu->decode.opcode_str, sizeof cpu->IQ_ENTRY[i].opcode_str);
            
            cpu->IQ_ENTRY[i].status_bit_IQ = 0;
            if(cpu->decode.opcode == OPCODE_MUL){
                cpu->IQ_ENTRY[i].fu_type = 2; // MUL->2
            }
            else if(cpu->decode.opcode == OPCODE_STORE && cpu->decode.opcode == OPCODE_STR && cpu->decode.opcode == OPCODE_LOAD && cpu->decode.opcode == OPCODE_LDR){
                cpu->IQ_ENTRY[i].fu_type = 3; //MEM->3
            }
            else{
                cpu->IQ_ENTRY[i].fu_type = 1;
            }
            cpu->IQ_ENTRY[i].imm = cpu->decode.imm;
            
            cpu->IQ_ENTRY[i].rs1_ready = checkIfRegIsValid(cpu->decode.rs1,cpu);
            cpu->IQ_ENTRY[i].rs1_tag = cpu->decode.rs1;
            cpu->IQ_ENTRY[i].rs1_value = cpu->decode.rs1_value;
            cpu->IQ_ENTRY[i].rs2_ready = checkIfRegIsValid(cpu->decode.rs2, cpu);
            cpu->IQ_ENTRY[i].rs2_tag = cpu->decode.rs2;
            cpu->IQ_ENTRY[i].rs2_value = cpu->decode.rs2_value;
            cpu->IQ_ENTRY[i].rs3_ready = checkIfRegIsValid(cpu->decode.rs3,cpu);
            cpu->IQ_ENTRY[i].rs3_tag = cpu->decode.rs3;
            cpu->IQ_ENTRY[i].rs3_value = cpu->decode.rs3_value;
            cpu->IQ_ENTRY[i].memory_address = cpu->decode.memory_address;
            cpu->ROB_TAIL=cpu->ROB_TAIL+1;
            printf("\n rs1=%d, rs2=%d, imm=%d\n",cpu->IQ_ENTRY[i].rs1_tag,cpu->IQ_ENTRY[i].rs2_tag,cpu->IQ_ENTRY[i].imm);
            break;


        }

    }



}

/*
 * Fetch Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */




static void
APEX_fetch(APEX_CPU *cpu)
{
    APEX_Instruction *current_ins;

    if (cpu->fetch.has_insn)
    {
        /* This fetches new branch target instruction from next cycle */
        if (cpu->fetch_from_next_cycle == TRUE)
        {
            cpu->fetch_from_next_cycle = FALSE;

            /* Skip this cycle*/
            return;
        }
        
        
        /* Store current PC in fetch latch */
        cpu->fetch.pc = cpu->pc;

        /* Index into code memory using this pc and copy all instruction fields
         * into fetch latch  */
        current_ins = &cpu->code_memory[get_code_memory_index_from_pc(cpu->pc)];
        strcpy(cpu->fetch.opcode_str, current_ins->opcode_str);
        cpu->fetch.opcode = current_ins->opcode;
        cpu->fetch.rd = current_ins->rd;
        cpu->fetch.rs1 = current_ins->rs1;
        cpu->fetch.rs2 = current_ins->rs2;
        cpu->fetch.rs3 = current_ins->rs3;
        cpu->fetch.imm = current_ins->imm;

        //if(cpu->stall_flag == FALSE){         //if stall_flag is true then stop fetching next instruction till dependency resolved.
        /* Update PC for next instruction */
        cpu->pc += 4;

        /* Copy data from fetch latch to decode latch*/
        cpu->decode = cpu->fetch;

        /* Stop fetching new instructions if HALT is fetched */
        if (cpu->fetch.opcode == OPCODE_HALT)
        {
            cpu->fetch.has_insn = FALSE;
        }
        //}
        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Fetch", &cpu->fetch);
        }
    }
}


//
///

static void 
APEX_ROB(APEX_CPU* cpu)
{
    if(cpu->decode.opcode == OPCODE_HALT)
    {
        cpu->ROB[cpu->ROB_TAIL].pc = cpu->decode.pc;
        cpu->ROB[cpu->ROB_TAIL].halt = 1;
        
     }
    else
    {
        cpu->ROB[cpu->ROB_TAIL].pc = cpu->decode.pc;
        cpu->ROB[cpu->ROB_TAIL].ar = cpu->decode.rd;
        cpu->ROB[cpu->ROB_TAIL].result = 0;
        cpu->ROB[cpu->ROB_TAIL].result_valid = 0;
        cpu->ROB[cpu->ROB_TAIL].fu_type= 1;
        cpu->ROB[cpu->ROB_TAIL].excodes= 0;
        cpu->ROB[cpu->ROB_TAIL].free=1;


        if(cpu->decode.opcode != OPCODE_CMP)
        {
           // printf("\n I am here 1 and cpu->ROB_TAIL:%d \n", cpu->ROB_TAIL);

            //printf("\n PC decode val:%d \n", cpu->decode.pc);
            cpu->ROB[cpu->ROB_TAIL].pc = cpu->decode.pc;
            cpu->ROB[cpu->ROB_TAIL].ar = cpu->decode.rd;
            cpu->ROB[cpu->ROB_TAIL].result = 0;
            cpu->ROB[cpu->ROB_TAIL].result_valid = 0;
            cpu->ROB[cpu->ROB_TAIL].fu_type= 1;
            cpu->ROB[cpu->ROB_TAIL].excodes= 0;
        }
    
    

        printf("\n %d \t",cpu->ROB[cpu->ROB_TAIL].pc);
        printf("%d \t",cpu->ROB[cpu->ROB_TAIL].ar);
        printf("%d \t",cpu->ROB[cpu->ROB_TAIL].result);
        printf("%d \t",cpu->ROB[cpu->ROB_TAIL].result_valid);
        printf("%d \t",cpu->ROB[cpu->ROB_TAIL].fu_type);
        printf("%d \t",cpu->ROB[cpu->ROB_TAIL].excodes);
        printf("%d \n",cpu->ROB_TAIL);

        if(cpu->ROB_TAIL==63)
        {
            cpu->ROB_TAIL=0;
        }
        else
        {
            cpu->ROB_TAIL++;
    
        }
    }   
                
}



///
//

/*
 * Decode Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_decode(APEX_CPU *cpu)
{
    if (cpu->decode.has_insn)
    {
        // stalling by checking valid_bit and setting stall_flag
            /*int i;
            for(i=0;i<16;i++)
            {
                printf("\n Valid bit %d is %d\n",i,arr_reg[i].valid_bit);
            }*/
        
        /* Read operands from register file based on the instruction type */
        APEX_ROB(cpu);
        switch (cpu->decode.opcode)
        {
          // logical operations
            case OPCODE_OR:
            case OPCODE_XOR:
            case OPCODE_AND:
            //Arithmatic operations
            case OPCODE_ADD:
            case OPCODE_SUB:
            case OPCODE_DIV:
            {

                // if(arr_reg[cpu->decode.rs1].valid_bit == FALSE){
                //  cpu->stall_flag = TRUE;
                //  }
                // if(arr_reg[cpu->decode.rs2].valid_bit == FALSE){
                // cpu->stall_flag = TRUE;
                // }

                // if(cpu->stall_flag == TRUE)
                // {
                //     break;
                // }
                
                
                cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
                //cpu->execute_INTU = cpu->decode;
                Populate_Issue_Queue(cpu);
                Issue_Queue(cpu);
                cpu->decode.has_insn = FALSE;
                arr_reg[cpu->decode.rd].valid_bit = FALSE;
                break;  
                
             
            }

            case OPCODE_MUL:
            {
                cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
                 //cpu->execute_MULU = cpu->decode;
                 //cpu->decode.has_insn = FALSE;
                   if(MULU_Occupied == TRUE)
                 {
                      cpu->stall_flag = TRUE;

                  }

                  if(cpu->stall_flag == FALSE)
                  {
                if(MULU_Occupied == FALSE){
                 //cpu->execute_MULU = cpu->decode;
                    Populate_Issue_Queue(cpu);
                    Issue_Queue(cpu);
                  cpu->decode.has_insn = FALSE;
                      }

              }
                arr_reg[cpu->decode.rd].valid_bit = FALSE;
                break;  
            }
            case OPCODE_LOAD:
            {
                // if(arr_reg[cpu->decode.rs1].valid_bit == FALSE){
                // cpu->stall_flag = TRUE;
                // }

                // if(cpu->stall_flag == TRUE)
                // {
                //     break;
                // }
                
                cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                //cpu->execute_MEMU = cpu->decode;
                Populate_Issue_Queue(cpu);
                Issue_Queue(cpu);
                cpu->decode.has_insn = FALSE;
                arr_reg[cpu->decode.rd].valid_bit = FALSE;
                break;
                
            }

            case OPCODE_MOVC:
            {
                /* MOVC doesn't have register operands */
                //cpu->execute_INTU = cpu->decode;
                Populate_Issue_Queue(cpu);
                Issue_Queue(cpu);
                cpu->decode.has_insn = FALSE;
                arr_reg[cpu->decode.rd].valid_bit = FALSE;
                break;
            }


            case OPCODE_ADDL:
            case OPCODE_SUBL:
            {
                // if(arr_reg[cpu->decode.rs1].valid_bit == FALSE){
                // cpu->stall_flag = TRUE;
                // }

                // if(cpu->stall_flag == TRUE)
                // {
                //     break;
                // }
                
                cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                 //cpu->execute_INTU = cpu->decode;
                 Populate_Issue_Queue(cpu);
                Issue_Queue(cpu);
                 cpu->decode.has_insn = FALSE;
                arr_reg[cpu->decode.rd].valid_bit = FALSE;
                break;
                
            }

            case OPCODE_CMP:
            {
                // if(arr_reg[cpu->decode.rs1].valid_bit == FALSE){
                //  cpu->stall_flag = TRUE;
                //  }
                // if(arr_reg[cpu->decode.rs2].valid_bit == FALSE){
                // cpu->stall_flag = TRUE;
                // }

                // if(cpu->stall_flag == TRUE)
                // {
                //    break;
                //}
                
                cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
                 // cpu->execute_INTU = cpu->decode;
                 Populate_Issue_Queue(cpu);
                Issue_Queue(cpu);
                cpu->decode.has_insn = FALSE;
                break;
                
            }

            case OPCODE_LDR:
            {
                // if(arr_reg[cpu->decode.rs1].valid_bit == FALSE){
                //  cpu->stall_flag = TRUE;
                //  }
                // if(arr_reg[cpu->decode.rs2].valid_bit == FALSE){
                // cpu->stall_flag = TRUE;
                // }
                
                // if(cpu->stall_flag == TRUE)
                // {
                //     break;
                // }
                cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
                //cpu->execute_MEMU = cpu->decode;
                Populate_Issue_Queue(cpu);
                Issue_Queue(cpu);
                cpu->decode.has_insn = FALSE;
                arr_reg[cpu->decode.rd].valid_bit = FALSE;
                break;
                
            }

            case OPCODE_STR:
            {
                // if(arr_reg[cpu->decode.rs1].valid_bit == FALSE){
                // cpu->stall_flag = TRUE;
                // }

                // if(arr_reg[cpu->decode.rs2].valid_bit == FALSE){
                //     cpu->stall_flag = TRUE;
                // }
                // if(arr_reg[cpu->decode.rs3].valid_bit == FALSE){
                //     cpu->stall_flag = TRUE;
                // } 

                // if(cpu->stall_flag == TRUE)
                // {
                //     break;
                // }
                
                cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
                cpu->decode.rs3_value = cpu->regs[cpu->decode.rs3];
               // cpu->execute_MEMU = cpu->decode;
               Populate_Issue_Queue(cpu);
               Issue_Queue(cpu);
                cpu->decode.has_insn = FALSE;
                break;
                
            }

            //need to check
            case OPCODE_STORE:
            {
                // if(arr_reg[cpu->decode.rs1].valid_bit == FALSE){
                // cpu->stall_flag = TRUE;
                // }

                // if(arr_reg[cpu->decode.rs2].valid_bit == FALSE){
                //     cpu->stall_flag = TRUE;
                // }
                
                // if(cpu->stall_flag == TRUE)
                // {
                //     break;
                // }

                cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
               // cpu->execute_MEMU = cpu->decode;
               Populate_Issue_Queue(cpu);
                Issue_Queue(cpu);
                cpu->decode.has_insn = FALSE;
                break;
                
            }
        }
        /* Copy data from decode latch to execute latch*/
        // if(cpu->stall_flag == FALSE)
        // {
       //  cpu->execute = cpu->decode;
        cpu->decode.has_insn = FALSE;
        

       // }
        

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Decode/RF", &cpu->decode);
        }
    }
}

/*
 * Execute Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_execute_INTU(APEX_CPU *cpu)
{
    //APEX_RetireINS(cpu);
    if (cpu->execute_INTU.has_insn)
    {
        /* Execute logic based on instruction type */
        switch (cpu->execute_INTU.opcode)
        {
            case OPCODE_ADD:
            {
                cpu->execute_INTU.result_buffer
                    = cpu->execute_INTU.rs1_value + cpu->execute_INTU.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute_INTU.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_BZ:
            {
                if (cpu->zero_flag == TRUE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->pc = cpu->execute_INTU.pc + cpu->execute_INTU.imm;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                     * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
                }
                break;
            }

            case OPCODE_BNZ:
            {
                if (cpu->zero_flag == FALSE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->pc = cpu->execute_INTU.pc + cpu->execute_INTU.imm;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                     * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
                }
                break;
            }

            case OPCODE_MOVC: 
            {
                cpu->execute_INTU.result_buffer = cpu->execute_INTU.imm;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute_INTU.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_SUB:
            {
                cpu->execute_INTU.result_buffer
                    = cpu->execute_INTU.rs1_value - cpu->execute_INTU.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute_INTU.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_DIV:
            {
                cpu->execute_INTU.result_buffer
                    = cpu->execute_INTU.rs1_value / cpu->execute_INTU.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute_INTU.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_OR:
            {
                cpu->execute_INTU.result_buffer
                    = cpu->execute_INTU.rs1_value || cpu->execute_INTU.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute_INTU.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_AND:
            {
                cpu->execute_INTU.result_buffer
                    = cpu->execute_INTU.rs1_value && cpu->execute_INTU.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute_INTU.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_XOR:
            {
                cpu->execute_INTU.result_buffer
                    = cpu->execute_INTU.rs1_value ^ cpu->execute_INTU.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute_INTU.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_ADDL:
            {
                cpu->execute_INTU.result_buffer
                    = cpu->execute_INTU.rs1_value + cpu->execute_INTU.imm;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute_INTU.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_SUBL:
            {
                cpu->execute_INTU.result_buffer
                    = cpu->execute_INTU.rs1_value - cpu->execute_INTU.imm;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute_INTU.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }
//changes
            case OPCODE_CMP:
            {
                /* Set the zero flag based on the result buffer */
                if (cpu->execute_INTU.rs1_value == cpu->execute_INTU.rs2_value)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }
        }

        /* Copy data from execute latch to memory latch*/
       // cpu->memory = cpu->execute_INTU;
       cpu->ROB[cpu->ROB_HEAD].result = cpu->execute_INTU.result_buffer;
        cpu->execute_INTU.has_insn = FALSE;
        cpu->ROB[cpu->ROB_HEAD].result_valid = 1;
        APEX_RetireINS(cpu, cpu->execute_INTU.result_buffer);
        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Execute", &cpu->execute_INTU);
        }
    }
}

static void 
APEX_execute_INTUV2(int rs1,int rs2, int robIdx, APEX_CPU *cpu)
{
    if (cpu->execute_INTU.has_insn)
    {
        int result;
        /* Execute logic based on instruction type */
        switch (cpu->execute_INTU.opcode)
        {
            case OPCODE_ADD:
            {
                
                result=rs1+rs2;
                /* Set the zero flag based on the result buffer */
                if (result == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_BZ:
            {
                if (cpu->zero_flag == TRUE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    //cpu->pc = cpu->execute_INTU.pc + cpu->execute_INTU.imm;
                    result=rs1+rs2;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                     * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
                }
                break;
            }

            case OPCODE_BNZ:
            {
                if (cpu->zero_flag == FALSE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->pc = cpu->execute_INTU.pc + cpu->execute_INTU.imm;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                     * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
                }
                break;
            }

            case OPCODE_MOVC: 
            {
                
                result=rs1;
                /* Set the zero flag based on the result buffer */
                if (result == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_SUB:
            {
                result=rs1-rs2;
                /* Set the zero flag based on the result buffer */
                if (result == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_DIV:
            {
                    result=rs1/rs2;

                /* Set the zero flag based on the result buffer */
                if (result == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_OR:
            {
                
                    result=rs1 || rs2;

                /* Set the zero flag based on the result buffer */
                if (result == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_AND:
            {

                    result= rs1 && rs2;

                /* Set the zero flag based on the result buffer */
                if (result == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_XOR:
            {
              result= rs1 ^ rs2;
                /* Set the zero flag based on the result buffer */
                if (result == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_ADDL:
            {
                result=rs1+rs2;

                /* Set the zero flag based on the result buffer */
                if (result == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_SUBL:
            {
              
                result=rs1-rs2;

                /* Set the zero flag based on the result buffer */
                if (result == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }
//changes
            case OPCODE_CMP:
            {
                /* Set the zero flag based on the result buffer */
                if (rs1 == rs2)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }
        }

        /* Copy data from execute latch to memory latch*/
       // cpu->memory = cpu->execute_INTU;
       cpu->ROB[robIdx].result = result;
        
        cpu->ROB[robIdx].result_valid = 1;
        APEX_RetireINS(cpu, result);
        cpu->execute_INTU.has_insn = FALSE;
        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Execute", &cpu->execute_INTU);
        }
    
}
}

static void
APEX_execute_MULUV2(int rs1, int rs2, int robIdx, APEX_CPU *cpu )
{

    if (cpu->execute_MULU.has_insn)
    {
        /* Execute logic based on instruction type */
        int result;
        if(MULU_Occupied == FALSE){
            
                MULU_counter = 1;
                MULU_Occupied = TRUE;
                result=rs1+rs2;
                /* Set the zero flag based on the result buffer */
                if (result == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                
        
        }
        else{
            if(MULU_counter != 2)
            {
                MULU_counter = MULU_counter+1;
            }
            else{
       // cpu->memory = cpu->execute_MULU;
        
        cpu->ROB[robIdx].result = result;
        cpu->ROB[robIdx].result_valid = 1;
        APEX_RetireINS(cpu, result);
        //stall flag flase
        cpu->execute_MULU.has_insn = FALSE;
        cpu->stall_flag = FALSE;
        MULU_Occupied = FALSE;
            }
        }

        /* Copy data from execute latch to memory latch*/
 

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Execute", &cpu->execute_MULU);
        }
    }


}


static void
APEX_execute_MULU(APEX_CPU *cpu)
{
    if (cpu->execute_MULU.has_insn)
    {
        /* Execute logic based on instruction type */
        
        if(MULU_Occupied == FALSE){
        switch (cpu->execute_MULU.opcode)
        {

            case OPCODE_MUL:
            {
                MULU_counter = 1;
                MULU_Occupied = TRUE;
                cpu->execute_MULU.result_buffer
                    = cpu->execute_MULU.rs1_value * cpu->execute_MULU.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute_MULU.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            
        }
        }
        else{
            if(MULU_counter != 2)
            {
                MULU_counter = MULU_counter+1;
            }
            else{
        cpu->memory = cpu->execute_MULU;
        cpu->execute_MULU.has_insn = FALSE;
        //stall flag flase
        cpu->stall_flag = FALSE;
        MULU_Occupied = FALSE;
            }
        }

        /* Copy data from execute latch to memory latch*/
 

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Execute", &cpu->execute_MULU);
        }
    }
}

static void
APEX_execute_MEMU(APEX_CPU *cpu)
{
    if (cpu->execute_MEMU.has_insn)
    {
        /* Execute logic based on instruction type */
        switch (cpu->execute_MEMU.opcode)
        {
            case OPCODE_LOAD:
            {
                cpu->execute_MEMU.memory_address
                    = cpu->execute_MEMU.rs1_value + cpu->execute_MEMU.imm;
                break;
            }

            case OPCODE_STORE:
            {
               
                cpu->execute_MEMU.memory_address
                    = cpu->execute_MEMU.rs2_value + cpu->execute_MEMU.imm;
                break;
            }

            case OPCODE_LDR:
            {
                cpu->execute_MEMU.result_buffer
                    = cpu->execute_MEMU.rs1_value + cpu->execute_MEMU.rs2_value;

                
                break;
            }

            case OPCODE_STR:
            {
                cpu->execute_MEMU.memory_address
                    = cpu->execute_MEMU.rs2_value + cpu->execute_MEMU.rs3_value;

                
                break;
            }
        }

        /* Copy data from execute latch to memory latch*/
        cpu->memory = cpu->execute_MEMU;
        cpu->execute_MEMU.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Execute", &cpu->execute_MEMU);
        }
    }
}


/*
 * Memory Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_memory(APEX_CPU *cpu)
{
    if (cpu->memory.has_insn)
    {
        switch (cpu->memory.opcode)
        {
            case OPCODE_ADD:
            {
                /* No work for ADD */
                break;
            }

            case OPCODE_LDR:
            case OPCODE_LOAD:
            {
                /* Read from data memory */
                cpu->memory.result_buffer
                    = cpu->data_memory[cpu->memory.memory_address];
                break;
            }
            case OPCODE_STORE:
            case OPCODE_STR:{
                  cpu->data_memory[cpu->memory.memory_address]
                    = cpu->memory.rs1_value;
                break;
            }
        }

        /* Copy data from memory latch to writeback latch*/
        cpu->writeback = cpu->memory;
        cpu->memory.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Memory", &cpu->memory);
        }
    }
}

/*
 * Writeback Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static int
APEX_writeback(APEX_CPU *cpu)
{
    if (cpu->writeback.has_insn)
    {
        /* Write result to register file based on instruction type */
        switch (cpu->writeback.opcode)
        { 
            // logical operations
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_XOR:
            // Arithmatic operations
            case OPCODE_ADD:
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
                arr_reg[cpu->writeback.rd].valid_bit = TRUE;
                //set stall_flag false to start fetching next instruction
                cpu->stall_flag = FALSE;
                break;
            }
            case OPCODE_LDR:
            case OPCODE_LOAD:
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
                arr_reg[cpu->writeback.rd].valid_bit = TRUE;
                //set stall_flag false to start fetching next instruction
                cpu->stall_flag = FALSE;
                break;
            }

            case OPCODE_MOVC: 
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
                arr_reg[cpu->writeback.rd].valid_bit = TRUE;
                //set stall_flag false to start fetching next instruction
                cpu->stall_flag = FALSE;
                break;
            }

            case OPCODE_SUB:
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
                arr_reg[cpu->writeback.rd].valid_bit = TRUE;
                //set stall_flag false to start fetching next instruction
                cpu->stall_flag = FALSE;
                break;
            }

            case OPCODE_MUL:
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
                arr_reg[cpu->writeback.rd].valid_bit = TRUE;
                cpu->stall_flag = FALSE;
                break;
            }

            case OPCODE_DIV:
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
                arr_reg[cpu->writeback.rd].valid_bit = TRUE;
                cpu->stall_flag = FALSE;
                break;
            }

            case OPCODE_ADDL:
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
                arr_reg[cpu->writeback.rd].valid_bit = TRUE;
                cpu->stall_flag = FALSE;
                break;
            }

            case OPCODE_SUBL:
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
                arr_reg[cpu->writeback.rd].valid_bit = TRUE;
                cpu->stall_flag = FALSE;
                break;
            }

            // no STR,STORE,CMP writeback stage
        }

        cpu->insn_completed++;
        cpu->writeback.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Writeback", &cpu->writeback);
        }

        if (cpu->writeback.opcode == OPCODE_HALT)
        {
            /* Stop the APEX simulator */
            return TRUE;
        }
    }

    /* Default */
    return 0;
}

/*
 * This function creates and initializes APEX cpu.
 *
 * Note: You are free to edit this function according to your implementation
 */
APEX_CPU *
APEX_cpu_init(const char *filename, const char *cmd)
{
    int i;
    APEX_CPU *cpu;

    if (!filename)
    {
        return NULL;
    }

    cpu = calloc(1, sizeof(APEX_CPU));

    if (!cpu)
    {
        return NULL;
    }

    int r;
    for(r=0; r<16; r++){
        arr_reg[r].reg_key = r;
        arr_reg[r].valid_bit = TRUE;
    }
    cpu->stall_flag = FALSE;
    /* Initialize PC, Registers and all pipeline stages */
    cpu->pc = 4000;
    memset(cpu->regs, 0, sizeof(int) * REG_FILE_SIZE);
    memset(cpu->data_memory, 0, sizeof(int) * DATA_MEMORY_SIZE);
    //cpu->single_step = ENABLE_SINGLE_STEP;



    if(strcmp(cmd,"display")==0){
        cpu->single_step = ENABLE_SINGLE_STEP;
    }
    else{
        cpu->single_step = 0;
    }

    /* Parse input file and create code memory */
    cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);
    if (!cpu->code_memory)
    {
        free(cpu);
        return NULL;
    }

    if (ENABLE_DEBUG_MESSAGES)
    {
        fprintf(stderr,
                "APEX_CPU: Initialized APEX CPU, loaded %d instructions\n",
                cpu->code_memory_size);
        fprintf(stderr, "APEX_CPU: PC initialized to %d\n", cpu->pc);
        fprintf(stderr, "APEX_CPU: Printing Code Memory\n");
        printf("%-9s %-9s %-9s %-9s %-9s\n", "opcode_str", "rd", "rs1", "rs2",
               "imm");

        for (i = 0; i < cpu->code_memory_size; ++i)
        {
            printf("%-9s %-9d %-9d %-9d %-9d\n", cpu->code_memory[i].opcode_str,
                   cpu->code_memory[i].rd, cpu->code_memory[i].rs1,
                   cpu->code_memory[i].rs2, cpu->code_memory[i].imm);
        }
    }

    /* To start fetch stage */
    cpu->fetch.has_insn = TRUE;
    return cpu;
}
void display(APEX_CPU *cpu)
{
    printf("\n=============== STATE OF ARCHITECTURAL REGISTER FILE ==========\n");
    int i;
    for(i=0;i<16;i++)
    {
    //    if(arr_reg[i].valid_bit == TRUE)

    //     printf("\n | REG[%d] | Value = %d | Status = VALID |\n",i,cpu->regs[i]);
    // else{
    printf("\n | REG[%d] | Value = %d | Status = VALID |\n",i,cpu->regs[i]);

    //}
    }
}
/*
 * APEX CPU simulation loop
 *
 * Note: You are free to edit this function according to your implementation
 */
void
APEX_cpu_run(APEX_CPU *cpu,int args_index)
{
    char user_prompt_val;

    while (TRUE)
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("--------------------------------------------\n");
            printf("Clock Cycle #: %d\n", cpu->clock);
            printf("--------------------------------------------\n");
        }

        if (APEX_writeback(cpu))
        {
            /* Halt in writeback stage */
            printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
            break;
        }

        APEX_memory(cpu);
        APEX_execute_INTU(cpu);
        APEX_execute_MULU(cpu);
        APEX_execute_MEMU(cpu);
        APEX_decode(cpu);
        APEX_fetch(cpu);

        print_reg_file(cpu);

        if (cpu->single_step)
        {
            printf("Press any key to advance CPU Clock or <q> to quit:\n");
            scanf("%c", &user_prompt_val);

            if ((user_prompt_val == 'Q') || (user_prompt_val == 'q'))
            {
                printf("APEX_CPU: Simulation Stopped, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
                break;
            }
        }
        if(cpu->clock==args_index){
            break;
        }
        cpu->clock++;

    }
    display(cpu);

    printf("\n============== STATE OF DATA MEMORY =============\n");
    int j;
    for(j=0;j<97;j++){
        printf("\n | MEM[%d] | Data Value = %d |\n",j, cpu->data_memory[j]);
    }

}

/*
 * This function deallocates APEX CPU.
 *
 * Note: You are free to edit this function according to your implementation
 */
void
APEX_cpu_stop(APEX_CPU *cpu)
{
    free(cpu->code_memory);
    free(cpu);
}