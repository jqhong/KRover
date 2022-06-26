
#include "centralhub.h"

#include <asm/ptrace.h>
#include <linux/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>

#include <map>
#include <vector>

// #include "BPatch.h"
// #include "BPatch_binaryEdit.h"
// #include "BPatch_flowGraph.h"
// #include "BPatch_function.h"


#include "centralhub.h"
#include "VMState.h"
#include "defines.h"
#include "fatctrl.h"
// #include "interface.h"
#include "thinctrl.h"

using namespace std;
// using namespace Dyninst;

/****************************** ExecState **************************/
ExecState::ExecState(ulong adds, ulong adde)
{
    m_VM.reset(new VMState());
    m_emeta.reset(new EveMeta);
    // exit(0);
    // // return;
    auto F = new CFattCtrl(m_VM.get(), m_emeta.get()); 
    auto T = new CThinCtrl(m_VM.get(), adds, adde);
    F->m_Thin = T;
    m_FattCtrl.reset(F);
    m_ThinCtrl.reset(T);

}

ExecState::~ExecState() {}

// bool ExecState::declareSymbolicObject(ulong addr, ulong size, const char *name) {
bool ExecState::declareSymbolicObject(ulong addr, ulong size, bool isSigned, long conVal, const char *name) {
    return m_VM->createSYMemObject(addr, size, isSigned, conVal, name);
}

// bool ExecState::declareSymbolicRegister(uint index, uint size, const char *name) {
bool ExecState::declareSymbolicRegister(uint index, uint size, bool isSigned, long conVal, const char *name) {
    return m_VM->createSYRegObject(index, size, isSigned, conVal, name);
}

// bool ExecState::SynRegsFromNative(struct pt_regs* regs)
bool ExecState::SynRegsFromNative(struct MacReg* regs)
{
    VMState::SetCPUState(m_VM.get(), regs);
    return true;
}

// bool ExecState::SynRegsToNative(struct pt_regs* regs)
bool ExecState::SynRegsToNative(struct MacReg* regs)
{
    VMState::ReadCPUState(m_VM.get(), regs);
    return true;
}

// bool ExecState::processAt(ulong addr, struct pt_regs *regs) {
bool ExecState::processAt(ulong addr) {
    
    // printf ("insn at addr %lx, are %lx, \n", addr, *((unsigned long*)addr));
    // // struct pt_regs* regs = m_VM->getPTRegs();
    struct MacReg* m_regs = (struct MacReg*) m_VM->getPTRegs();
    printf ("rax: %lx, rdi:%lx, rsi: %lx, rdx: %lx. \n", m_regs->regs.rax, m_regs->regs.rdi, m_regs->regs.rsi, m_regs->regs.rdx);
    unsigned long tmp = m_regs->regs.rdi;
    tmp += 0x60;
    printf ("nice value: %d. \n", *((unsigned long*)tmp));
    printf ("fs_base: %lx, gs_base:%lx . \n", m_regs->fs_base, m_regs->gs_base);
    
    // return true;
    // asm volatile ("movq $0xfff, %%rax; \n\t"
    //         "vmcall; \n\t"
    //         :::"%rax");
   
    
    // // declareSymbolicObject(tmp, 4, 19, "niceval_rdx"); 
    // // declareSymbolicObject(tmp, 8, 19, "niceval_rdx"); 
    // declareSymbolicObject(tmp, 8, 1, 19, "niceval_rdx"); 
    // tmp += 0x8;
    // // declareSymbolicObject(tmp, 4, 0x0, "who_rsi"); 
    // // declareSymbolicObject(tmp, 8, 0x0, "who_rsi"); 
    // declareSymbolicObject(tmp, 8, 1, 0x0, "who_rsi"); 
    // // // printf ("who : %d. \n", *((unsigned long*)tmp));
    // tmp += 0x8;
    // // // // // // printf ("which : %d. \n", *((unsigned long*)tmp));
    // // declareSymbolicObject(tmp, 4, 0x0, "which_rdi"); 
    // // declareSymbolicObject(tmp, 8, 0x0, "which_rdi"); 
    // declareSymbolicObject(tmp, 8, 1, 0x0, "which_rdi"); 
    // // declareSymbolicObject(tmp, 4, 0x3, "which_rdi"); 

    // declareSymbolicObject(tmp, 8); 
   
    // asm volatile ("movq $0xfff, %%rax; \n\t"
    //         "vmcall; \n\t"
    //         :::"%rax");
    return m_FattCtrl->processFunc(addr);
}

bool ExecState::MoniStartOfSE(ulong addr) {
    return m_FattCtrl->MoniStartOfSE(addr);
}

void ExecState::InitRediPagePool() {
    return m_FattCtrl->InitRediPagePool();
}

void ExecState::DBHandler() {
    return m_FattCtrl->DBHandler();
}
// /******************************exported for external**************************/
// // CFacade *gHub = nullptr;
// ExecState *es = nullptr;
// 
// EXPORT_ME bool oasis_lib_init(ulong adds, ulong adde) {
//     if (es != nullptr)
//         delete es;
//     es = new ExecState(adds, adde);
// 
//     return true;
// }
// 
// EXPORT_ME void oasis_lib_fini(void) {
//     if (es != nullptr)
//         delete es;
// }
// 
// EXPORT_ME bool StartExecutionAt(ulong addr, struct pt_regs *regs) {
//     // if (gHub == nullptr) {
//     if (es == nullptr) {
//         cout << "invoke oasis_lib_init first to do system initialization\n";
//         exit(EXIT_FAILURE);
//     }
//     return es->processAt(addr, regs);
// }
// 
// EXPORT_ME bool DeclareSymbolicObject(ulong addr, ulong size) {
//     if (es == nullptr) {
//         cout << "invoke oasis_lib_init first to do system initialization\n";
//         exit(EXIT_FAILURE);
//     }
//     return es->declareSymbolicObject(addr, size);
// }

/* Jiaqi */
/* /Jiaqi */

// Module initialization and finalization
__attribute__((constructor)) void module_init(void) {
    // cout << __PRETTY_FUNCTION__ << "\n";
}

__attribute__((destructor)) void module_fini(void) {
    // cout << __PRETTY_FUNCTION__ << "\n";
}
