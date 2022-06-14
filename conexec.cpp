#include "conexec.h"

#include <linux/types.h>
#include <sys/mman.h>
#include <signal.h>
#include <ucontext.h>

#include <iostream>

#include "VMState.h"
#include "interface.h"
#include "symexec.h"

using namespace std;
using namespace Dyninst;
using namespace ParseAPI;
using namespace InstructionAPI;

extern "C" void InsnExecNonRIP (struct pt_regs* regs);
void InsnExecNonRIP (struct pt_regs* regs);
asm (" .text");
asm (" .type    InsnExecNonRIP, @function");
// asm (" .align 1024");
asm (" .align 4096");
asm ("InsnExecNonRIP: \n");
/* save Ana context on stack */
asm ("pushq %rax \n"); 
asm ("pushq %rbx \n");
asm ("pushq %rcx \n");
asm ("pushq %rdx \n");
asm ("pushq %rdi \n");//0x58
asm ("pushq %rsi \n");//0x50
asm ("pushq %rbp \n");//0x48
asm ("pushq %r8 \n");//0x40
asm ("pushq %r9 \n");//0x38
asm ("pushq %r10 \n");//0x30
asm ("pushq %r11 \n");//0x28
asm ("pushq %r12 \n");//0x20
asm ("pushq %r13 \n");//0x18
asm ("pushq %r14 \n");//0x10
asm ("pushq %r15 \n");//0x8
asm ("pushf \n");//0x0
asm ("movq %rsp, 0xcf(%rip) \n");//Save A-rsp
/* load target context */
asm ("movq (%rdi), %r15 \n");//addr of pt_regs 
asm ("movq 0x8(%rdi), %r14 \n");
asm ("movq 0x10(%rdi), %r13 \n");
asm ("movq 0x18(%rdi), %r12 \n");
asm ("movq 0x20(%rdi), %rbp \n");
asm ("movq 0x28(%rdi), %rbx \n");
asm ("movq 0x30(%rdi), %r11 \n");
asm ("movq 0x38(%rdi), %r10 \n");
asm ("movq 0x40(%rdi), %r9 \n");
asm ("movq 0x48(%rdi), %r8 \n");
asm ("movq 0x50(%rdi), %rax \n");
asm ("movq 0x58(%rdi), %rcx \n");
asm ("movq 0x60(%rdi), %rdx \n");
asm ("movq 0x68(%rdi), %rsi \n");
asm ("push 0x90(%rdi) \n");
asm ("popf \n");
asm ("movq 0x98(%rdi), %rsp \n");
asm ("movq 0x70(%rdi), %rdi \n");
/* 15-byte nop for T instruction */
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
// asm ("movq $0xabcdabcd, %rax \n");
// asm ("vmcall \n");
/* save T context */
// asm ("movq %rsp, 0x7f(%rip) \n");//Save T-rsp
// asm ("movq 0x70(%rip), %rsp \n");//Load A-rsp
asm ("xchg 0x70(%rip), %rsp \n");//Load A-rsp
asm ("push %rdi \n");
asm ("movq 0x60(%rsp), %rdi \n");//addr of pt_regs 
asm ("movq %r15, (%rdi) \n");
asm ("movq %r14, 0x8(%rdi) \n");
asm ("movq %r13, 0x10(%rdi) \n");
asm ("movq %r12, 0x18(%rdi) \n");
asm ("movq %rbp, 0x20(%rdi) \n");
asm ("movq %rbx, 0x28(%rdi) \n");
asm ("movq %r11, 0x30(%rdi) \n");
asm ("movq %r10, 0x38(%rdi) \n");
asm ("movq %r9 , 0x40(%rdi) \n");
asm ("movq %r8 , 0x48(%rdi) \n");
asm ("movq %rax, 0x50(%rdi) \n");
asm ("movq %rcx, 0x58(%rdi) \n");
asm ("movq %rdx, 0x60(%rdi) \n");
asm ("movq %rsi, 0x68(%rdi) \n");
asm ("pop %rsi \n");
asm ("movq %rsi, 0x70(%rdi) \n");//save T-rdi
asm ("pushf \n");
asm ("pop 0x90(%rdi) \n");
asm ("movq 0x20(%rip), %rsi \n");//saved T-rsp
asm ("movq %rsi, 0x98(%rdi) \n");
/* Restore Ana context */
asm ("popf \n");
asm ("popq %r15 \n");
asm ("popq %r14 \n");
asm ("popq %r13 \n");
asm ("popq %r12 \n");
asm ("popq %r11 \n");
asm ("popq %r10 \n");
asm ("popq %r9 \n");
asm ("popq %r8 \n");
asm ("popq %rbp \n");
asm ("popq %rsi \n");
asm ("popq %rdi \n");
asm ("popq %rdx \n");
asm ("popq %rcx \n");
asm ("popq %rbx \n");
asm ("popq %rax \n");
asm ("retq \n");
asm ("nop \n");//saved Ana-RSP/T-RSP 8-byte 
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
// asm ("nop \n");//T-RSP 8-byte
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");

//assume RIP Relative instruction only read rip, do not write rip
//so, load r15 with rip; after execution, no need to update r15 and rip
extern "C" void InsnExecRIP (struct pt_regs* regs);
void InsnExecRIP (struct pt_regs* regs);
asm (" .text");
asm (" .type    InsnExecRIP, @function");
asm ("InsnExecRIP: \n");
/* save Ana context on stack */
asm ("pushq %rax \n"); 
asm ("pushq %rbx \n");
asm ("pushq %rcx \n");
asm ("pushq %rdx \n");
asm ("pushq %rdi \n");//0x58
asm ("pushq %rsi \n");//0x50
asm ("pushq %rbp \n");//0x48
asm ("pushq %r8 \n");//0x40
asm ("pushq %r9 \n");//0x38
asm ("pushq %r10 \n");//0x30
asm ("pushq %r11 \n");//0x28
asm ("pushq %r12 \n");//0x20
asm ("pushq %r13 \n");//0x18
asm ("pushq %r14 \n");//0x10
asm ("pushq %r15 \n");//0x8
asm ("pushf \n");//0x0
asm ("movq %rsp, 0xd0(%rip) \n");//Save A-rsp
/* load target context */
asm ("movq 0x80(%rdi), %r15 \n");//rdi stores addr of pt_regs, load r15 with rip 
// asm ("vmcall \n");
asm ("movq 0x8(%rdi), %r14 \n");
asm ("movq 0x10(%rdi), %r13 \n");
asm ("movq 0x18(%rdi), %r12 \n");
asm ("movq 0x20(%rdi), %rbp \n");
asm ("movq 0x28(%rdi), %rbx \n");
asm ("movq 0x30(%rdi), %r11 \n");
asm ("movq 0x38(%rdi), %r10 \n");
asm ("movq 0x40(%rdi), %r9 \n");
asm ("movq 0x48(%rdi), %r8 \n");
asm ("movq 0x50(%rdi), %rax \n");
asm ("movq 0x58(%rdi), %rcx \n");
asm ("movq 0x60(%rdi), %rdx \n");
asm ("movq 0x68(%rdi), %rsi \n");
asm ("push 0x90(%rdi) \n");
asm ("popf \n");
asm ("movq 0x98(%rdi), %rsp \n");
asm ("movq 0x70(%rdi), %rdi \n");
/* 15-byte nop for T instruction */
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
// asm ("movq $0xabcdabcd, %rax \n");
// asm ("vmcall \n");
/* save T context */
// asm ("movq %rsp, 0x7f(%rip) \n");//Save T-rsp
// asm ("movq 0x70(%rip), %rsp \n");//Load A-rsp
asm ("xchg %rsp, 0x6d(%rip) \n");//restore A-rsp, save T-rsp
asm ("push %rdi \n");
asm ("movq 0x60(%rsp), %rdi \n");//addr of pt_regs 
// asm ("movq %r15, (%rdi) \n");//no need to update r15
asm ("movq %r14, 0x8(%rdi) \n");
asm ("movq %r13, 0x10(%rdi) \n");
asm ("movq %r12, 0x18(%rdi) \n");
asm ("movq %rbp, 0x20(%rdi) \n");
asm ("movq %rbx, 0x28(%rdi) \n");
asm ("movq %r11, 0x30(%rdi) \n");
asm ("movq %r10, 0x38(%rdi) \n");
asm ("movq %r9 , 0x40(%rdi) \n");
asm ("movq %r8 , 0x48(%rdi) \n");
asm ("movq %rax, 0x50(%rdi) \n");
asm ("movq %rcx, 0x58(%rdi) \n");
asm ("movq %rdx, 0x60(%rdi) \n");
asm ("movq %rsi, 0x68(%rdi) \n");
asm ("pop %rsi \n");
asm ("movq %rsi, 0x70(%rdi) \n");//save T-rdi
asm ("pushf \n");
asm ("pop 0x90(%rdi) \n");
asm ("movq 0x20(%rip), %rsi \n");//saved T-rsp
asm ("movq %rsi, 0x98(%rdi) \n");
/* Restore Ana context */
asm ("popf \n");
asm ("popq %r15 \n");
asm ("popq %r14 \n");
asm ("popq %r13 \n");
asm ("popq %r12 \n");
asm ("popq %r11 \n");
asm ("popq %r10 \n");
asm ("popq %r9 \n");
asm ("popq %r8 \n");
asm ("popq %rbp \n");
asm ("popq %rsi \n");
asm ("popq %rdi \n");
asm ("popq %rdx \n");
asm ("popq %rcx \n");
asm ("popq %rbx \n");
asm ("popq %rax \n");
asm ("retq \n");
asm ("nop \n");//saved Ana-RSP/T-RSP 8-byte 
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
asm ("nop \n");
// asm ("nop \n");//T-RSP 8-byte
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");

// bool ConExecutor::bind_value_for_exp(Instruction* instr, Expression::Ptr target, struct pt_regs* regs)
// {
//     // std::set<RegisterAST::Ptr> regsRead;
//     // // (*(operands.begin())).getReadSet(regsRead);
//     // instr->getReadSet(regsRead);
// 
//     std::vector<Operand> operands;
//     instr->getOperands (operands);
//     Address tempTarget;
//     for (auto iter = operands.begin(); iter != operands.end(); iter++)
//     {
//         // cout << (*iter).format(Arch_x86_64, 0) << endl;
//         std::set<RegisterAST::Ptr> regsRead;
//         (*iter).getReadSet(regsRead);
//         for (auto iter = regsRead.begin(); iter != regsRead.end(); iter++)
//         {
//             // printf ("id: %lx. \n", ((*iter)->getID()));
//             switch ((*iter)->getID())
//             {
//                 // signed int GPR = 0x00010000
//                 RegisterAST* rast;
//                 case Arch_x86_64+0x10000 : //rax
//                     rast = new RegisterAST(x86_64::rax);
//                     // tempTarget = target_ctx->rax;
//                     tempTarget = regs->rax;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x10001 : //rcx
//                     rast = new RegisterAST(x86_64::rcx);
//                     // tempTarget = target_ctx->rcx;
//                     tempTarget = regs->rcx;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x10002 : //rdx
//                     rast = new RegisterAST(x86_64::rdx);
//                     // tempTarget = target_ctx->rdx;
//                     tempTarget = regs->rdx;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x10003 : //rbx
//                     rast = new RegisterAST(x86_64::rbx);
//                     // tempTarget = target_ctx->rbx;
//                     tempTarget = regs->rbx;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x10004 : //rsp
//                     rast = new RegisterAST(x86_64::rsp);
//                     // tempTarget = target_ctx->rsp;
//                     tempTarget = regs->rsp;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x10005 : //rbp
//                     rast = new RegisterAST(x86_64::rbp);
//                     // tempTarget = target_ctx->rbp;
//                     tempTarget = regs->rbp;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x10006 : //rsi
//                     rast = new RegisterAST(x86_64::rsi);
//                     // tempTarget = target_ctx->rsi;
//                     tempTarget = regs->rsi;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x10007 : //rdi
//                     rast = new RegisterAST(x86_64::rdi);
//                     // tempTarget = target_ctx->rdi;
//                     tempTarget = regs->rdi;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x10008 : //r8
//                     rast = new RegisterAST(x86_64::r8);
//                     // tempTarget = target_ctx->r8;
//                     tempTarget = regs->r8;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x10009 : //r9
//                     rast = new RegisterAST(x86_64::r9);
//                     // tempTarget = target_ctx->r9;
//                     tempTarget = regs->r9;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x1000a : //r10
//                     rast = new RegisterAST(x86_64::r10);
//                     // tempTarget = target_ctx->r10;
//                     tempTarget = regs->r10;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x1000b : //r11
//                     rast = new RegisterAST(x86_64::r11);
//                     // tempTarget = target_ctx->r11;
//                     tempTarget = regs->r11;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x1000c : //r12
//                     rast = new RegisterAST(x86_64::r12);
//                     // tempTarget = target_ctx->r12;
//                     tempTarget = regs->r12;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x1000d : //r13
//                     rast = new RegisterAST(x86_64::r13);
//                     // tempTarget = target_ctx->r13;
//                     tempTarget = regs->r13;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x1000e : //r14
//                     rast = new RegisterAST(x86_64::r14);
//                     // tempTarget = target_ctx->r14;
//                     tempTarget = regs->r14;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
//                 case Arch_x86_64+0x1000f : //r15
//                     rast = new RegisterAST(x86_64::r15);
//                     // tempTarget = target_ctx->r15;
//                     tempTarget = regs->r15;
//                     target->bind(rast, Result(s64, tempTarget));
//                     break;
// 
//                 // case Arch_x86_64+0x10010 : //rip
//                 case Arch_x86_64+0x00010 : //rip
//                     rast = new RegisterAST(x86_64::rip);
//                     // crtAddr = board_ctx->rip;
//                     // crtAddr = crtAddr + instr->size(); 
//                     // target->bind(rast, Result(s64, crtAddr));
//                     // tempTarget = crtAddr;
//                     tempTarget = regs->rip;
//                     // printf ("tempTarget: %lx. \n", tempTarget);
//                     tempTarget += instr->size();
//                     // printf ("instruction length: %lx. tempTarget: %lx. \n", instr->size(), tempTarget);
//                     target->bind(rast, Result(s64, tempTarget));
//                     // printf ("rip. \n");
//                     break;
// 
//                 default:
//                     asm volatile ("vmcall; \n\t");
//             }
//         }
//     }
//     return true;
// }
// 
// bool ConExecutor::if_condition_fail (entryID opera_id, struct pt_regs* regs)
// {
//     bool ret;
//     unsigned long eflags;
//     // eflags = target_ctx->eflags;
//     eflags = regs->eflags;
//     bool cf, pf, zf, sf, of;
//     cf = eflags & 0x1;
//     pf = (eflags >> 2) & 0x1;
//     zf = (eflags >> 6) & 0x1;
//     sf = (eflags >> 7) & 0x1;
//     of = (eflags >> 11) & 0x1;
// 
//     ret = false;
//    
//     /* the index operation is in dyninst/common/h/entryIDs.h */
//     // switch (instr->getOperation().getID())
//     switch (opera_id)
//     {
//         case e_jnbe:
//         // if (cf && zf)
//             if (cf || zf)
//                 ret = true;
//                 break;
//         case e_jb: 
//             if (!cf)
//                 ret = true;
//                 break;
//         case e_jnb: 
//             if (cf)
//                 ret = true;
//                 break;
//         case e_jnb_jae_j: 
//             if (cf)
//                 ret = true;
//                 break;
//         case e_jb_jnaej_j:
//             if (!cf)
//                 ret = true;
//                 break;
//         case e_jbe:
//             if ((!cf) && (!zf))
//                 ret = true;
//                 break;
//         case e_jz:
//             if (!zf)
//                 ret = true;
//                 break;
//         case e_jnz:
//             if (zf)
//                 ret = true;
//                 break;
// 
//         case e_jnp:
//             if (pf)
//                 ret = true;
//                 break;
//         case e_jp: 
//             if (!pf)
//                 ret = true;
//                 break;
//         case e_jcxz_jec:
//             int ecx;
//             // ecx = (int) (board_ctx->rcx & 0xffffffff);
//             ecx = (int) (regs->rcx & 0xffffffff);
//             // printf ("jcx instruction. crtAddr: %lx. ecx: %lx rcx: %lx . \n", crtAddr, ecx, board_ctx->rcx);
//             // asm volatile ("vmcall; \n\t");
//             if (ecx)
//                 ret = true;
//                 break;
//     /* singed conditional jumps */
//         case e_jnle:
//         // if (zf && (sf ^ of))
//             if (zf || (sf ^ of))
//                 ret = true;
//                 break;
//         case e_jnl:
//             if ((sf ^ of))
//                 ret = true;
//                 break;
//         case e_jl:
//             if (!(sf ^ of))
//                 ret = true;
//                 break;
//         case e_jle:
//         // if (!((sf ^ of) && zf))
//             if (!((sf ^ of) || zf))
//                 ret = true;
//                 break;
//         case e_jno:
//             if (of)
//                 ret = true;
//                 break;
//         case e_jns:
//             if (sf)
//                 ret = true;
//                 break;
//         case e_jo:
//             if (!of)
//                 ret = true;
//                 break;
//         case e_js: 
//             if (!sf)
//                 ret = true;
//                 break;
//         default :
//             printf ("////conditional jump. curAddr: %lx \n", crtAddr);
//             asm volatile ("vmcall; \n\t");
//     }
// 
//     return ret;
// }
// 
// /* emulate the effect of control flow instruction */
// bool ConExecutor::emul_cf_inst(Instruction* instr, InsnCategory Cate, struct pt_regs* regs)
// {
//     bool ret = false;
//     
//     switch (Cate) {
//         case c_ReturnInsn: 
//             {
//                 Address temp_rsp = regs->rsp;
//                 Address tempTarget = *((unsigned long*) temp_rsp);
//       
//                 regs->rip = tempTarget;
//                 regs->rsp += 0x8;
//                 crtAddr = tempTarget;
//                 
//                 ret = true;
//                 break;
//             }
//         case c_CallInsn: 
//             {
//                 // cout << "\"" << instr->format() << endl;
//                 Expression::Ptr target = instr->getControlFlowTarget();
//                 if (target)
//                 {
//                     RegisterAST* rast = new RegisterAST(MachRegister::getPC(Arch_x86_64));
//                     target->bind(rast, Result(s64, crtAddr));
//                     Result res = target->eval();
//                     Address tempTarget;
//                     if (res.defined) //direct call
//                     {
//                         tempTarget = res.convert<Address>();
//                         // printf ("call, target1: %lx. target2: %lx. rsp: %lx. \n", bb_recording[k_u_indicator][crt_bb_idx].target1, bb_recording[k_u_indicator][crt_bb_idx].target2, board_ctx->rsp);
//                     }
//                     else // indirect call 
//                     {
//                         bind_value_for_exp(instr, target, regs);
//                         res = target->eval();
//                         if (!(res.defined))//get target address from memory
//                         {
//                             std::vector<Expression::Ptr> temp_exp;
//                             target->getChildren(temp_exp);
// 
//                             target = *(temp_exp.begin());
//                             res = target->eval();
//                             
//                             tempTarget = res.convert<Address>();
//                             tempTarget = *((unsigned long*) tempTarget);
//                         }
//                         else
//                         {
//                             tempTarget = res.convert<Address>();
//                         }
//                     }
//                 
//                     /* skip the call instruction */
//                     // board_ctx->rip = tempTarget;
//                     // target_ctx->rsp -= 0x8;
//                     // *((unsigned long*) (target_ctx->rsp)) = crtAddr + instr->size();
//                     regs->rip = tempTarget;
//                     regs->rsp -= 0x8;
//                     *((unsigned long*) (regs->rsp)) = crtAddr + instr->size();
//                     crtAddr = tempTarget;
//                     
//                     ret = true;
//                 }
//                 else
//                 {
//                     printf ("control flow target for call Insn cannot be resolved \n");
//                     ret = false;
//                 }
//                 break;
//             }
//         case c_BranchInsn: 
//             {
//                 // cout << "\"" << instr->format() << endl;
//                 Expression::Ptr target = instr->getControlFlowTarget();
//                 if (target)
//                 {
//                     RegisterAST* rast = new RegisterAST(MachRegister::getPC(Arch_x86_64));
//                     target->bind(rast, Result(s64, crtAddr));
//                     Result res = target->eval();
//                     Address tempTarget;
//                     if (res.defined)//direct jmp
//                     {
//                         tempTarget = res.convert<Address>();
//                     }
//                     else//indirect jmp
//                     {
//                         bind_value_for_exp(instr, target, regs);
//                         res = target->eval();
//                         if (!(res.defined))//get target address from memory
//                         {
//                             std::vector<Expression::Ptr> temp_exp;
//                             target->getChildren(temp_exp);
// 
//                             target = *(temp_exp.begin());
//                             res = target->eval();
//                             
//                             tempTarget = res.convert<Address>();
//                             tempTarget = *((unsigned long*) tempTarget);
//                         }
//                         else
//                         {
//                             tempTarget = res.convert<Address>();
//                         }
//                     }
//                    
//                     /* handle the second possible destination if it is conditional jump */
//                     if (instr->allowsFallThrough())
//                     {
//                         // asm volatile ("vmcall; \n\t");
//                         entryID temp_operation_id = instr->getOperation().getID();
//                         int ret = if_condition_fail(temp_operation_id, regs);
//                         if (ret)//if condition fail, change tempTarget to next instruction
//                         {
//                             tempTarget = crtAddr + instr->size();
//                         }
//                     }
// 
//                     /* skip the jmp instruction */
//                     regs->rip = tempTarget;
//                     crtAddr = tempTarget;
//                     printf ("temp_target in jmp instruction: %lx. rdx: %lx, rax: %lx. crtAddr: %lx, :%lx. \n", tempTarget, regs->rdx, regs->rax, crtAddr, *((unsigned long*) (crtAddr)));
//                     ret = true;
//                 }
//                 else
//                 {
//                     printf ("control flow target for call Insn cannot be resolved \n");
//                     ret = false;
//                 }
//                 break;
//             }
//         default: 
//             {
//                 printf ("Insn is not a control flow insn \n");
//                 ret = false;
//                 break;
//             }
//     }
//     return ret;
// }

// bool ConExecutor::RewRIPInsn(void* new_insn, char* orig_insn, Instruction* instr)
bool ConExecutor::RewRIPInsn(void* orig_insn_addr, Instruction* instr)
{
    // char orig_insn[7];
    // char orig_insn[8];
    char orig_insn[12];
    // orig_insn[7] = '\n';
    // void* T_insn_no_r15 = (void*)((char*)InsnExecRIP + 0x6e);
    // memcpy (orig_insn, orig_insn_addr, 0x7);
    // memcpy (orig_insn, orig_insn_addr, 0x8);
    void* T_insn_no_r15 = (void*)((char*)InsnExecRIP + 0x6c);
    
    entryID opcode = instr->getOperation().getID();

    // printf ("T_insn_no_r15 at: %lx. \n", T_insn_no_r15);
        
    static Expression::Ptr x86_gs(new RegisterAST(x86_64::gs));
  
    if (opcode == e_mov || opcode == e_cmp || opcode == e_lea || opcode == e_add || opcode == e_sub)
    {
        memcpy (orig_insn, orig_insn_addr, 0x8);
    
        if (instr->isRead(x86_gs))
        {
            // printf ("gs base rip rel insn. \n");
            if (opcode == e_mov || opcode == e_add)
            {
                if (instr->size() == 7)
                {
                    memcpy (orig_insn+2, orig_insn_addr+0x1, 6);
                    orig_insn[1] = 0x41;
                }
                else if(instr->size() == 8)
                {
                    orig_insn[1] |= 0x1;
                }
                else
                {
                    printf ("non recognised gs base insn. \n");
                    asm volatile ("vmcall; \n\t");
                }
                //handling orig_insn[3]
                if (orig_insn[4] == 0 && orig_insn[5] == 0 && orig_insn[6] == 0  && orig_insn[7] == 0)
                {
                    orig_insn[3] |= 0x02;
                    memcpy(T_insn_no_r15, orig_insn, 4);
                }
                else if (orig_insn[4] != 0 && orig_insn[5] == 0 && orig_insn[6] == 0  && orig_insn[7] == 0) 
                {
                    orig_insn[3] |= 0x42;
                    memcpy(T_insn_no_r15, orig_insn, 5);
                }
                else
                {
                    orig_insn[3] |= 0x82;
                    memcpy(T_insn_no_r15, orig_insn, 8);
                }
                // unsigned long* tmp = (unsigned long*)T_insn_no_r15;
                // printf ("new rip-relative insn with gs base: %lx. \n", *tmp);
            }
            else
            {
                asm volatile ("vmcall; \n\t");
            }

        }
        else
        {
            if (instr->size() == 7)//both operand size are 8-byte
            {
                orig_insn[0] |= 0x1;
                if (orig_insn[3] == 0 && orig_insn[4] == 0 && orig_insn[5] == 0  && orig_insn[6] == 0)
                {
                    orig_insn[2] |= 0x02;
                    memcpy(T_insn_no_r15, orig_insn, 3);
                }
                else if (orig_insn[3] != 0 && orig_insn[4] == 0 && orig_insn[5] == 0  && orig_insn[6] == 0) 
                {
                    orig_insn[2] |= 0x42;
                    memcpy(T_insn_no_r15, orig_insn, 4);
                }
                else
                {
                    orig_insn[2] |= 0x82;
                    memcpy(T_insn_no_r15, orig_insn, 7);
                }
            }
            else if (instr->size() == 6)//the non-rip rel operand is 4-byte
            {
                memcpy (orig_insn+1, orig_insn_addr, 6);
                orig_insn[0] = 0x41;
                if (orig_insn[3] == 0 && orig_insn[4] == 0 && orig_insn[5] == 0  && orig_insn[6] == 0)
                {
                    orig_insn[2] |= 0x02;
                    memcpy(T_insn_no_r15, orig_insn, 3);
                }
                else if (orig_insn[3] != 0 && orig_insn[4] == 0 && orig_insn[5] == 0  && orig_insn[6] == 0) 
                {
                    orig_insn[2] |= 0x42;
                    memcpy(T_insn_no_r15, orig_insn, 4);
                }
                else
                {
                    orig_insn[2] |= 0x82;
                    memcpy(T_insn_no_r15, orig_insn, 7);
                }

            }
            else
            {
                asm volatile ("vmcall; \n\t");
            }
        }
    }
    else if (opcode == e_and && instr->isRead(x86_gs))
    { 
        if (instr->size() == 11)
        {
            memcpy (orig_insn, orig_insn_addr, 0x1);
            memcpy (orig_insn+2, orig_insn_addr+0x1, 10);
            orig_insn[1] = 0x41;
            // currently, only handles when the displacement is 32-bit
            if (orig_insn[4] != 0 && orig_insn[5] != 0 && orig_insn[6] != 0)
            {
                orig_insn[3] |= 0x82;
                memcpy(T_insn_no_r15, orig_insn, 12);
            }
            else
            {
                assert(0);//TO FIX: For rip relative operand, the displacement is alway 4-byte long even if the displacement(offset) is 0. But if use r15 as base, the size for displacement is based on the displacement value, can be 0-bit, 8-bit, 16-bit or 32-bit. So, when change rip-->r15, be careful about the displacement size.

            }
            // unsigned long* tmp = (unsigned long*)T_insn_no_r15;
            // printf ("new rip-relative and insn with gs base: %lx. \n", *tmp);
            // printf ("new rip-relative and insn with gs base: %lx. \n", *(tmp+1));
        }
        else
        {
            assert(0);
        }
    }
    else if (opcode == e_xadd)
    {
        if (instr->size() == 8)
        {
            memcpy (orig_insn, orig_insn_addr, 0x1);
            memcpy (orig_insn+2, orig_insn_addr+1, 0x7);
            orig_insn[1] = 0x41;
            if (orig_insn[5] != 0 && orig_insn[6] != 0 && orig_insn[7] != 0)
            {
                orig_insn[4] |= 0x82;
                memcpy(T_insn_no_r15, orig_insn, 9);
            }
            else
            {
                assert(0);
            }
        }
        else
        {
            assert(0);
        }
    }
    else if (opcode == e_test)
    {
        memcpy ((void*)&orig_insn[1], orig_insn_addr, 0x7);
           
        orig_insn[0] = 0x41;
        if (orig_insn[4] == 0 && orig_insn[5] == 0 && orig_insn[6] == 0  && orig_insn[7] == 0)
        {
            orig_insn[2] |= 0x02;
            memcpy(T_insn_no_r15, orig_insn, 4);
        }
        else if (orig_insn[4] != 0 && orig_insn[5] == 0 && orig_insn[6] == 0  && orig_insn[7] == 0) 
        {
            orig_insn[2] |= 0x42;
            memcpy(T_insn_no_r15, orig_insn, 5);
        }
        else
        {
            orig_insn[2] |= 0x82;
            memcpy(T_insn_no_r15, orig_insn, 8);
        }
        // unsigned long* tmp = (unsigned long*)T_insn_no_r15;
        // printf ("new rip-relative test insn: %lx. \n", *tmp);

    }
    else
    {
        printf ("rip-relative instruction, type not handled. \n");
        asm volatile ("movq $0x999999, %%rax; \n\t"
                "vmcall; \n\t"
                :::"%rax");
    }
    return true;
}
    
bool ConExecutor::ClearTinsn(void* T_addr, int size)
{
    // unsigned long* tmp = (unsigned long*)&NopBytes[0];
    // printf ("nop bytes: %lx. \n", *tmp);
    
    memcpy(T_addr, (void*)&NopBytes[0], size);
    return true;
}

/* InsnDispatch: execute one Insn per time, update T_RIP. Different return value
 * is to facilate Testing */
bool ConExecutor::InsnDispatch(Instruction* instr, struct pt_regs* regs)
{
    int InsnSize = instr->size();
    // printf ("crtAddr: %lx, insn cate: %d. size: %d. \n", crtAddr, cate, InsnSize);
    
    // struct pt_regs* regs = m_regs;
    // asm volatile ("movq $0x999999, %%rax; \n\t"
    //         "vmcall; \n\t"
    //         :::"%rax");
    ulong crtAddr = regs->rip - InsnSize; 
    /* For RIP relative instruction: if r15 is not used in instruction, replace rip with r15 */
    Expression::Ptr thePC(new RegisterAST(MachRegister::getPC(Arch_x86_64)));
    if (instr->isRead(thePC))
    {
        Expression::Ptr theR15(new RegisterAST(MachRegister(x86_64::r15)));
        if (instr->isRead(theR15) || instr->isWritten(theR15))
        {
            printf ("r15 is used, use another reg . \n");
            asm volatile ("movq $0x999999, %%rax; \n\t"
                    "vmcall; \n\t"
                    :::"%rax");
        }
        else
        {
            /* TODO: currently only handle cmp/mov/lea/sub/add which are
             * 7-bytes */
            // printf ("rip relative instruction, addr: %lx. \n", crtAddr);
            
            RewRIPInsn((void*)crtAddr, instr);
            InsnExecRIP(regs);
            void* T_insn_no_r15 = (void*)((char*)InsnExecRIP + 0x6c);
            // ClearTinsn(T_insn_no_r15, 8);
            ClearTinsn(T_insn_no_r15, 12);
            
            // std::cout << "rdi: " << std::hex << regs->rdi << std::endl;
        }
    }
    /* For non-rip-relative instruction, */
    else 
    {
        // printf ("non rip relative instruction, addr: %lx. \n", crtAddr);
        // std::cout << "rdi: " << regs->rdi << std::endl;
        // std::cout << "rax: " << regs->rax << std::endl;
        void* T_insn = (void*)((char*)InsnExecNonRIP + 0x68);
        // std::cout << "T_insn at: " << T_insn << std::endl;
        unsigned long* tmp = (unsigned long*) T_insn;
        
        memcpy(T_insn, (void*)crtAddr, InsnSize);
        
        // printf ("copy T insn: %lx. \n", *tmp);
        
        InsnExecNonRIP(regs);
        ClearTinsn(T_insn, InsnSize);

        // printf ("restore nop: %lx. \n", *tmp);
       
        // std::cout << "rdi: " << regs->rdi << std::endl;
    }
    
    return 0;
}

// extern "C" void BlockExecRIP (void);
// void BlockExecRIP (void);
// asm (" .text");
// asm (" .type    BlockExecRIP, @function");
// asm ("BlockExecRIP: \n");
// /* save Ana context on stack */
// asm ("pushq %rax \n"); 
// asm ("pushq %rbx \n");
// asm ("pushq %rcx \n");
// asm ("pushq %rdx \n");
// asm ("pushq %rdi \n");
// asm ("pushq %rsi \n");
// asm ("pushq %rbp \n");
// asm ("pushq %r8 \n");
// asm ("pushq %r9 \n");
// asm ("pushq %r10 \n");
// asm ("pushq %r11 \n");
// asm ("pushq %r12 \n");
// asm ("pushq %r13 \n");
// asm ("pushq %r14 \n");
// // asm ("pushq %r15 \n");
// asm ("pushf \n");
// asm ("movq %rsp, 0xdd(%rip) \n");//save ana rsp
// /* load target context */
// asm ("movq $0x7f7fffffec48, %rax \n");//addr of target_ctx
// asm ("movq (%rax), %rbx \n");
// asm ("pushq %rbx \n");
// asm ("popf \n");
// // asm ("movq 0x8(%rax), %r15 \n");
// asm ("movq 0x10(%rax), %r14 \n");
// asm ("movq 0x18(%rax), %r13 \n");
// asm ("movq 0x20(%rax), %r12 \n");
// asm ("movq 0x28(%rax), %rbp \n");
// asm ("movq 0x30(%rax), %rbx \n");
// asm ("movq 0x38(%rax), %r11 \n");
// asm ("movq 0x40(%rax), %r9 \n");
// asm ("movq 0x48(%rax), %r8 \n");
// asm ("movq 0x50(%rax), %r10 \n");
// asm ("movq 0x58(%rax), %rdx \n");
// asm ("movq 0x60(%rax), %rsi \n");
// asm ("movq 0x68(%rax), %rdi \n");
// asm ("movq 0x70(%rax), %rsp \n");
// asm ("movq 0x80(%rax), %rax \n");
// asm ("movq 0x88(%rax), %rcx \n");
// /* jmp to T_page. 8-nop to store the addr of T_page */
// asm ("jmpq *(%rip) \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// /* BlkExec_RET: save T context */
// asm ("movq %rax, 0x7f(%rip) \n");
// asm ("movq $0x7f7fffffec48, %rax \n");//addr of target_ctx
// // asm ("movq %r15, 0x8(%rax) \n");
// asm ("pushf \n");
// asm ("popq %r15 \n");
// asm ("movq %r15, (%rax) \n");//save eflags
// asm ("movq %r14, 0x10(%rax) \n");
// asm ("movq %r13, 0x18(%rax) \n");
// asm ("movq %r12, 0x20(%rax) \n");
// asm ("movq %rbp, 0x28(%rax) \n");
// asm ("movq %rbx, 0x30(%rax) \n");
// asm ("movq %r11, 0x38(%rax) \n");
// asm ("movq %r9, 0x40(%rax) \n");
// asm ("movq %r8, 0x48(%rax) \n");
// asm ("movq %r10, 0x50(%rax) \n");
// asm ("movq %rdx, 0x58(%rax) \n");
// asm ("movq %rsi, 0x60(%rax) \n");
// asm ("movq %rdi, 0x68(%rax) \n");
// asm ("movq %rsp, 0x70(%rax) \n");
// asm ("movq %rcx, 0x88(%rax) \n");
// asm ("movq 0x2d(%rip), %rcx \n");//save T-RAX
// asm ("movq %rcx, 0x80(%rax) \n");
// /* Restore Ana stack & Load Ana context */
// asm ("movq 0x17(%rip), %rsp\n");//restore ana rsp
// asm ("popf \n");
// // asm ("popq %r15 \n");
// asm ("popq %r14 \n");
// asm ("popq %r13 \n");
// asm ("popq %r12 \n");
// asm ("popq %r11 \n");
// asm ("popq %r10 \n");
// asm ("popq %r9 \n");
// asm ("popq %r8 \n");
// asm ("popq %rbp \n");
// asm ("popq %rsi \n");
// asm ("popq %rdi \n");
// asm ("popq %rdx \n");
// asm ("popq %rcx \n");
// asm ("popq %rbx \n");
// asm ("popq %rax \n");
// asm ("retq \n");
// asm ("nop \n");//Ana-RSP 8-byte 
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");//T-RSP 8-byte
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// asm ("nop \n");
// 
// /* S_Addr: starting addr of T_Insn that has not been copied;
//  * crtAddr: addr to be disassembled;
//  * T_Insn are copied to T_page; 
//  * nt_t_page: next addr on T_page where the following T_Insn are copied to;
//  * if encounter RIP-relative Insn: (involve Insn ReWrite) copy S_Addr until crtAddr
//  * if encounter CF Insn: copy S_Addr until crtAddr and execute */
// ConExecutor::BlockDispatch(Address S_Addr, Address E_Addr) {
// // void BlockDispatch(Address S_Addr, Address E_Addr)
// {
//     crtAddr = S_Addr;
//     void* nt_t_page = T_page;
//     int R15_flag = 0;
//     InsnCategory cate;
//     int InsnSize;
//     int total_len;//total len to be copied
//     unsigned long BlockExec_Ret = (unsigned long)(BlockExecRIP + 0x7c);
// 
//     do {
//         instr = decoder->decode((unsigned char *)cr->getPtrToInstruction(crtAddr));
//         cate = instr->getCategory();
//         InsnSize = instr->size();
//         printf ("in BlockExec. crtAddr: %lx, insn cate: %d. size: %d. \n", crtAddr, cate, InsnSize);
//         /* For Control Flow Insn, execute all T-Insn by now */
//         if (cate == c_ReturnInsn || cate == c_CallInsn || cate == c_BranchInsn)
//         {
//             printf ("exec one blk due to control flow insn .\n");
//             total_len = crtAddr - S_Addr;
//             memcpy(nt_t_page, (void*)S_Addr, total_len);
//             nt_t_page += total_len;
//             memcpy(nt_t_page, Jmp_RIP_Insn, 6);//copy the jmp insn at the end
//             nt_t_page += 6;
//             *((unsigned long*)nt_t_page) = BlockExec_Ret;
//             BlockExecRIP();
//             emul_cf_inst(cate);
//             S_Addr = crtAddr;//Since Insn until the CF Insn has been executed, update S_Addr
//             nt_t_page = T_page;
//             R15_flag = 0;
//         }
//        
//         else 
//         {
//             std::vector<Operand> operands;
//             instr->getOperands (operands);
//             for (auto iter = operands.begin(); iter != operands.end(); iter++)
//             {
//                 std::set<RegisterAST::Ptr> regsRead;
//                 std::set<RegisterAST::Ptr> regsWrite;
//                 (*iter).getReadSet(regsRead);
//                 (*iter).getWriteSet(regsWrite);
//                 int regID;
//                 /* For RIP relative Insn, RIP should not be a write Reg, only
//                  * check if R15 is in use */
//                 for (auto it = regsWrite.begin(); it != regsWrite.end(); it++)
//                 {
//                     regID = (*it)->getID();
//                     if (regID == (Arch_x86_64+0x1000f))
//                     {
//                         R15_flag += 1;
//                     }
//                 }
//                 for (auto it = regsRead.begin(); it != regsRead.end(); it++)
//                 {
//                     regID = (*it)->getID();
//                     if (regID == (Arch_x86_64+0x1000f))
//                     {
//                         R15_flag += 1;
//                     }
//                     /* For RIP-Rel Insn, copy all Insn to T-Page */
//                     if (regID == (Arch_x86_64+0x10010))
//                     {
//                         if (R15_flag != 0)
//                         {
//                             printf ("R15 is used: %d. \n", R15_flag);
//                             asm volatile ("movq $0x999999, %%rax; \n\t"
//                                     "vmcall; \n\t"
//                                     :::"%rax");
//                         }
//                         else
//                         {
//                             total_len = crtAddr - S_Addr;
//                             memcpy(nt_t_page, (void*)S_Addr, total_len);
//                             nt_t_page += total_len;
//                             char orig_insn[7];
//                             memcpy (orig_insn, (void*)crtAddr, 0x7);
//                             RewRIPInsn(orig_insn);
//                             memcpy(nt_t_page, orig_insn, 0x7);
//                             nt_t_page += 0x7;
//                             S_Addr = crtAddr + InsnSize;
//                             goto nt_insn;
//                         }
//                     }
//                 }
//             }
// nt_insn:
//             crtAddr += InsnSize;
//         }
//     } while (crtAddr < E_Addr);
// 
//     /* If there are Insn left on T_page... */ 
//     if (nt_t_page != T_page)
//     {
//         /* copy Insn if there are, then execute */ 
//         if (crtAddr != S_Addr)
//         {
//             total_len = crtAddr - S_Addr;
//             printf ("copy remanining Insn. S_Addr: %lx, crtAddr: %lx. \n", S_Addr, crtAddr);
//             memcpy(nt_t_page, (void*)S_Addr, total_len);
//             nt_t_page += total_len;
//         }
//         printf ("exec remanining Insn. nt_t_page: %lx. \n", nt_t_page);
//         memcpy(nt_t_page, Jmp_RIP_Insn, 6);//copy the jmp insn at the end
//         nt_t_page += 6;
//         *((unsigned long*)nt_t_page) = BlockExec_Ret;
//         BlockExecRIP();
//         nt_t_page = T_page;
//         R15_flag = 0;
//     }
//     // asm volatile ("movq $0x999999, %%rax; \n\t"
//     //         "vmcall; \n\t"
//     //         :::"%rax");
//   
//     return; 
// }

ConExecutor::ConExecutor() 
{
    /* Make the three ConcretExecutor pages writable */
    int ret;
    void* execPage = (void*)(((unsigned long)InsnExecNonRIP) & ~0xFFF);
    ret = mprotect(execPage, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
    execPage = (void*)(((unsigned long)InsnExecRIP) & ~0xFFF);
    ret = mprotect(execPage, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
    // Following are to handle block concrete exec, comment first 
    // execPage = (void*)(((unsigned long)BlockExecRIP) & ~0xFFF);
    // ret = mprotect(execPage, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
    // printf ("ret of mprotect: %d. \n", ret);
    //    
    // /* Init a RIP-relative Jmp Insn which is the last Insn of Block T-Insn Executor */
    // auto init = std::initializer_list<unsigned char>({0xff, 0x25, 0x00, 0x00, 0x00, 0x00});
    // std::copy(init.begin(), init.end(), Jmp_RIP_Insn);
    // T_page = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE | MAP_POPULATE, -1, 0);
    // if (T_page == MAP_FAILED)
    // {
    //     printf ("Init T_page failed: %lx. \n", T_page);
    //     asm volatile ("movq $0xffff, %rax; \n\t"
    //             "vmcall; \n\t");
    // }
    // /* Init the addr of T_page on BlockExec */
    // unsigned long* BlkExec_T_page = (unsigned long*)(BlockExecRIP + 0x74);
    // *BlkExec_T_page = (unsigned long)T_page;
    // /* / */
}

