#include <iostream>
#include "Expr.h"
#include "oprand.h"

#include "VMState.h"

using namespace EXPR;
static char exprName[][16] = {"","","","","","","",
                            "Add" ,	
                            "Sub" ,	
                            "Mul" ,	
                            "UDiv" ,	
                            "SDiv" ,	
                            "URem" ,	
                            "SRem" ,	
                            "Neg" ,     
                            "Not" ,	
                            "And" ,	
                            "Or	" ,	 
                            "Xor" ,	
                            "Shl" ,	
                            "LShr" ,	
                            "AShr" ,	
                            "Equal" ,
                            "Distinct" ,
                            "Ult" ,	
                            "Ule" ,	
                            "Ugt" ,	
                            "Uge" ,	
                            "Slt" ,	
                            "Sle" ,	
                            "Sgt" ,	
                            "Sge" ,	
                            "LOr" ,	
                            "LAnd" ,
                            "LNot" ,
                            "SignEXT",
                            "ZeroEXT",
                            "Shrd",
                            "Sign", 
                            "NoSign",
                            "Overflow",
                            "NoOverflow",
                            "Combine",
                            "Extract",
                            "CombineMulti"
                            };

void UDefExpr::print () {
    VMState::SYMemObject *obj = (VMState::SYMemObject *) O;
    
    if(obj != NULL )
        std::cout << obj->name ;
    else
        std::cout << "OBJ" ;
}

void ConstExpr::print () {
    std::cout << "0x" << std::hex << V;
}

void BinExpr::print () {
    std::cout<<exprName[exprID]<<"("; 
    R->print(); 
    std::cout<<",";
    L->print();
    std::cout<<")";
}

void UryExpr::print () {
    std::cout<<exprName[exprID]<<"("; 
    E->print(); 
    std::cout<<")";
}

void TriExpr::print () {
    std::cout<<exprName[exprID]<<"("; 
    R->print(); 
    std::cout<<",";
    M->print(); 
    std::cout<<",";
    L->print();
    std::cout<<")";
}

void ExtractExpr::print () {
    
    std::cout<<exprName[exprID]<<"("; 
    E->print();
    std::cout << ", " << s << ", " << e ;
    std::cout<<")";
}

void CombineMultiExpr::print () {
    
    std::cout<<exprName[exprID]<<"("; 
    for (auto e : exprs) {
        e->print() ;
        std::cout << ", " ;
    }
    std::cout<<")";
}

KVExprPtr CreateExprByID(int id, KVExprPtr R, KVExprPtr M, KVExprPtr L, int size = 4, int offset = 0) {
    BinExpr *binptr = (BinExpr *)R.get() ;
    KVExprPtr exprptr = NULL;
    switch (id) {
        case EXPR_Add		 :
        case EXPR_Sub		 :
        case EXPR_Mul		 :
        case EXPR_UDiv		 :
        case EXPR_SDiv		 :
        case EXPR_URem		 :
        case EXPR_SRem		 :
        case EXPR_Neg        :
        case EXPR_Not		 :
        case EXPR_And		 :
        case EXPR_Or		 :
        case EXPR_Xor		 :
        case EXPR_Shl		 :
        case EXPR_LShr		 :
        case EXPR_AShr		 :
            asm ("int3") ;
            break ;

        case EXPR_Equal		 :
            exprptr.reset(new EqualExpr(R, size, offset)) ;
            break ;

        case EXPR_Distinct   :
            exprptr.reset(new DistinctExpr(R, size, offset)) ;
            break ;

        case EXPR_Ult		 :
            exprptr.reset(new UltExpr(binptr->getR(), binptr->getL(), size, offset)) ;
            break ;

        case EXPR_Ule		 :
            exprptr.reset(new UleExpr(binptr->getR(), binptr->getL(), size, offset)) ;
            break ;

        case EXPR_Ugt		 :
            exprptr.reset(new UgtExpr(binptr->getR(), binptr->getL(), size, offset)) ;
            break ;

        case EXPR_Uge		 :
            exprptr.reset(new UgeExpr(binptr->getR(), binptr->getL(), size, offset)) ;
            break ;

        case EXPR_Slt		 :
            exprptr.reset(new SltExpr(binptr->getR(), binptr->getL(), size, offset)) ;
            break ;

        case EXPR_Sle		 :
            exprptr.reset(new SleExpr(binptr->getR(), binptr->getL(), size, offset)) ;
            break ;

        case EXPR_Sgt		 :
            exprptr.reset(new SgtExpr(binptr->getR(), binptr->getL(), size, offset)) ;
            break ;

        case EXPR_Sge		 :
            exprptr.reset(new SgeExpr(binptr->R, binptr->L, size, offset)) ;
            break ;

        case EXPR_LOr		 :
        case EXPR_LAnd		 :
        case EXPR_LNot		 :
        case EXPR_SignEXT    :
        case EXPR_ZeroEXT    :
        case EXPR_Shrd       :
            asm ("int3") ;
            break ;
        default :
            break ;
    }
    return exprptr ;
}