#include "Z3Handler.h"

#include <limits.h>
#include <stdlib.h>

#include "VMState.h"

// print color setting
#define COLOR_NONE "\033[0m"
#define RED "\033[1;31;40m"


void perror(char *const msg);
void perror(char *const msg){ //TODO
    printf("\033[47;31m Z3 Handlering ERROR : %s \033[0m", msg);
    printf("\n");
}

using namespace z3;
using namespace Z3HANDLER;

static z3::context g_z3_context;
static z3::solver g_solver(g_z3_context);
// z3::context g_z3_context;
// z3::solver g_solver(g_z3_context);

Z3Handler::Z3Handler() : context_(g_z3_context){}

Z3Handler::~Z3Handler(){}

/* Input : a set of constraints
 * Output: a map which stores the symbolic variables and its solved concrte value
 */
std::map<std::string, unsigned long long> Z3Handler::Z3SolveOne(std::set<KVExprPtr> constraints){
    std::map<std::string, unsigned long long> ret;
    g_solver.reset();
    expr exprs = context_.bool_val(1);
    for (auto it = constraints.begin(); it != constraints.end(); it++){
        exprs = exprs and Z3HandlingExprPtr(*it);
        //expr expr_temp = Z3HandlingExprPtr(*it);
        //g_solver.add(expr_temp);
    }
    g_solver.add(exprs);
    // std::cout << "solver: " << g_solver <<  std::endl;
    // produce .smt2 file which can be used in other solvers
    //std::cout << "smt2 :" << g_solver.to_smt2() << "\n done" << std::endl;
    switch (g_solver.check()){
        case sat: {
                // std::cout << "SAT" << std::endl;
                model m = g_solver.get_model();
                // std::cout << "m size " << m.size() << std::endl;
                for (unsigned i = 0; i < m.size(); i++) {
                    func_decl v = m[i];
                    // this problem contains only constants
                    assert(v.arity() == 0);
                    //std::cout << v.name() << " = " << m.get_const_interp(v) << "\n";
                    ret.insert(std::pair<std::string, unsigned long long>(v.name().str(), m.get_const_interp(v).get_numeral_uint64()));
                }
                break;
                }
        case unsat:{
                std::cout << "UNSAT" << std::endl;
                printf("\033[47;31m Oops, constraints are unsolvable ... ! \033[0m\n");
                break;
                }
        case unknown:{
                std::cout << "UNKNOWN" << std::endl;
                printf("\033[47;31m Oops, constraints are unsolvable ... ! \033[0m\n");
                break;
                }
    }
    return ret;
}

/*
 * Function: check whether a constraint can be true when concritizing a symbolic variable
 * Input: 1) Symbolic object defined in SYMemObject*; 2)the concrete value; 3) a set of constraints
 * Output: a bool value : true/false
 *
*/
bool Z3Handler::Z3SolveConcretize(std::vector<VMState::SYMemObject*> symobjs, std::set<KVExprPtr> constraints){
    bool ret;
    //z3::solver Solver(context_);
    symObjectsMap.clear(); // reset map to be null
    g_solver.reset();
    expr exprs = context_.bool_val(1);
    for (auto it = constraints.begin(); it != constraints.end(); it++){
        exprs = exprs & Z3HandlingExprPtr(*it);
    }
    g_solver.add(exprs);
    // std::cout << "checking sat/unsat before concritization: " << g_solver.check() << std::endl;

    // std::cout << g_solver << std::endl;
    // std::cout << "++++++++++++++" << std::endl;

    // if (symobjs.size() != values.size()) {
    //     printf("\033[47;31m Z3 Handlering ERROR : The number of corresponding symbols and values under concritization is not the same! \033[0m\n");
    //     exit(1); //shoud we just exit?
    // }
    for (int i = 0; i < symobjs.size(); i++){
        // checking whether the input obj exists in the corrent constraints, raise an error if no;
        if (symObjectsMap.find(symobjs[i]) == symObjectsMap.end()) {
            printf("\033[47;31m Z3 Handlering ERROR : The input symbolic object is not in the current constraints! \033[0m\n");
            //throw symobjs[i];
        }
        // otherwise, get the symbolic expr in Z3 and add the extra constraint
        // expr value_expr = context_.bv_val(values[i], 32); // any bad effects when it's 64-bit?
        // expr value_expr = context_.bv_val(symobjs[i]->i32, 32); // any bad effects when it's 64-bit?
        expr value_expr = context_.bv_val(symobjs[i]->i64, 64); // any bad effects when it's 64-bit?
        // std::cout << "-------------" << symobjs[i]->i32 << std::endl;
        for (auto it = symObjectsMap.begin(); it != symObjectsMap.end(); it ++){
            if (it->first == symobjs[i]) {
                // std::cout << "-------------" << std::endl;
                expr sym_expr = it->second;
                // exprs = exprs and (sym_expr == value_expr);
                // break;
                g_solver.add(sym_expr == value_expr);
            }
        }
    }
    // std::cout << "checking sat/unsat after concritization: " << g_solver.check() << std::endl;
    // std::cout << g_solver << std::endl;
    if (g_solver.check() == sat)
        ret = true;
    else
        ret = false;
    return ret;
}

z3::expr Z3Handler::Z3HandlingExprPtr(ExprPtr ptr){
    int kind = ptr->getKind();

    switch (kind){
        case Expr::Kind::UNDEFINED:{ // -1
            //std::cout << "Handling " << kind << std::endl;
            return Z3HandleUND(ptr);
        }
        case Expr::Kind::Const:{ // 3
            return Z3HandleConst(ptr);
        }
        case Expr::Kind::Bin:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::Tri:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::Ury:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::Add: { // 7
            AddExpr * expr_r = static_cast<AddExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleAdd(R, L);
        }
        case Expr::Kind::Sub:{
            SubExpr * expr_r = static_cast<SubExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleSub(R, L);
        }
        case Expr::Kind::Mul:{
            MulExpr * expr_r = static_cast<MulExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleMul(R, L);
        }
        case Expr::Kind::UDiv:{
            DivExpr * expr_r = static_cast<DivExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleUDiv(R, L);
        }
        case Expr::Kind::SDiv:{ // 11
            iDivExpr * expr_r = static_cast<iDivExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleSDiv(R, L);
        }
        case Expr::Kind::URem:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::SRem:{
            iRemExpr * expr_r = static_cast<iRemExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleSRem(R, L);
        }
        case Expr::Kind::Neg:{
            return Z3HandleNeg(ptr);
        }
        case Expr::Kind::Not:{
            return Z3HandleNot(ptr);
        }
        case Expr::Kind::And:{
            AndExpr * expr_r = static_cast<AndExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleAnd(R, L);
        }
        case Expr::Kind::Or:{
            OrExpr * expr_r = static_cast<OrExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleOr(R, L);
        }
        case Expr::Kind::Xor:{
            XorExpr * expr_r = static_cast<XorExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleXor(R, L);
        }
        case Expr::Kind::Shl:{
            Shl_SalExpr * expr_r = static_cast<Shl_SalExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleShl(R, L);
        }
        case Expr::Kind::LShr:{
            ShrExpr * expr_r = static_cast<ShrExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleLShr(R, L);
        }
        case Expr::Kind::AShr:{ // 21
            SarExpr * expr_r = static_cast<SarExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleAShr(R, L);
        }
        case Expr::Kind::Equal:{
            return Z3HandleEqual(ptr);
        }
        case Expr::Kind::Distinct:{
            return Z3HandleDistinct(ptr);
        }
        case Expr::Kind::Ult:{
            BinExpr * e = static_cast<BinExpr*>(ptr.get());
            ExprPtr R = e->getExprPtrR();
            ExprPtr L = e->getExprPtrL();
            return Z3HandleUlt(R, L);
        }
        case Expr::Kind::Ule:{
            BinExpr * e = static_cast<BinExpr*>(ptr.get());
            ExprPtr R = e->getExprPtrR();
            ExprPtr L = e->getExprPtrL();
            return Z3HandleUle(R, L);
        }
        case Expr::Kind::Ugt:{
            BinExpr * e = static_cast<BinExpr*>(ptr.get());
            ExprPtr R = e->getExprPtrR();
            ExprPtr L = e->getExprPtrL();
            return Z3HandleUgt(R, L);
        }
        case Expr::Kind::Uge:{
            BinExpr * e = static_cast<BinExpr*>(ptr.get());
            ExprPtr R = e->getExprPtrR();
            ExprPtr L = e->getExprPtrL();
            return Z3HandleUge(R, L);
        }
        case Expr::Kind::Slt:{
            BinExpr * e = static_cast<BinExpr*>(ptr.get());
            ExprPtr R = e->getExprPtrR();
            ExprPtr L = e->getExprPtrL();
            return Z3HandleSlt(R, L);
        }
        case Expr::Kind::Sle:{
            BinExpr * e = static_cast<BinExpr*>(ptr.get());
            ExprPtr R = e->getExprPtrR();
            ExprPtr L = e->getExprPtrL();
            return Z3HandleSle(R, L);
        }
        case Expr::Kind::Sgt:{
            BinExpr * e = static_cast<BinExpr*>(ptr.get());
            ExprPtr R = e->getExprPtrR();
            ExprPtr L = e->getExprPtrL();
            return Z3HandleSgt(R, L);
        }
        case Expr::Kind::Sge:{ // 31
            BinExpr * e = static_cast<BinExpr*>(ptr.get());
            ExprPtr R = e->getExprPtrR();
            ExprPtr L = e->getExprPtrL();
            return Z3HandleSge(R, L);
        }
        case Expr::Kind::Lor:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::LAnd:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::LNot:{
            expr ex = Z3HandleLNot(ptr);
            return ex; // TODO need to confirm further
        }
        case Expr::Kind::SignEXT:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::ZeroEXT:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::Shrd:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::Sign:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::NoSign:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::Overflow:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::NoOverflow:{ // 41
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? \033[0m\n");
            throw ptr;
        }
        case Expr::Kind::Combine:{
            CombineExpr * expr_r = static_cast<CombineExpr*>(ptr.get());
            ExprPtr R = expr_r->getExprPtrR();
            ExprPtr L = expr_r->getExprPtrL();
            return Z3HandleCombine(R, L);
        }
        case Expr::Kind::Extract:{
            return Z3HandleExtract(ptr);
        }
        default:{
            printf("\033[47;31m Z3 Handlering ERROR : Unsupported type of EXPR? No such type! \033[0m\n");
            throw ptr;
        }
    }
}


z3::expr Z3Handler::Z3HandleUND(ExprPtr udef_expr_ptr){
    UDefExpr * udef_expr = static_cast<UDefExpr*>(udef_expr_ptr.get());
    if (udef_expr == NULL){
        printf("\033[47;31m Z3 Handlering ERROR : UDefExpr \033[0m\n");
        throw udef_expr;
    }
    VMState::SYMemObject *obj = udef_expr->getObject();
    std::string sym_name = obj->name;
    unsigned long long size = obj->size;
    expr x = context_.bv_const(sym_name.c_str(), size*8);
    symObjectsMap.insert(std::pair<VMState::SYMemObject*, z3::expr>(obj, x));
    return x;
}

z3::expr Z3Handler::Z3HandleConst(ExprPtr const_expr_ptr){ // 3
    ConstExpr *const_expr = static_cast<ConstExpr*>(const_expr_ptr.get());
    if (const_expr == NULL){
        printf("\033[47;31m Z3 Handlering ERROR : UDefExpr \033[0m\n");
        throw const_expr;
    }
    uint64_t value = const_expr->getValue();
    //std::cout << "value in ConstExpr : " << value << std::endl;
    expr x = context_.bv_val(value, 32);
    return x;
}

z3::expr Z3Handler::Z3HandleBin(ExprPtr r, ExprPtr l){ // not sure how to write z3 expr
    return context_.bv_val(100, 64);
}

z3::expr Z3Handler::Z3HandleTri(ExprPtr r, ExprPtr m, ExprPtr l){ // not sure how to write z3 expr
    return context_.bv_val(100, 64);
}

z3::expr Z3Handler::Z3HandleUry(ExprPtr ptr){ // not sure how to write z3 expr
    expr x = context_.bv_const("x", 100);
    return x;
}

z3::expr Z3Handler::Z3HandleAdd(ExprPtr r, ExprPtr l){ // 7
    //context c;
    expr ret = (Z3HandlingExprPtr(r) + Z3HandlingExprPtr(l));
    //TODO need to handle signed/unsigned ?
    return ret;
}

z3::expr Z3Handler::Z3HandleSub(ExprPtr r, ExprPtr l){
    expr ret = (Z3HandlingExprPtr(r) - Z3HandlingExprPtr(l));
    return ret;
}

z3::expr Z3Handler::Z3HandleMul(ExprPtr r, ExprPtr l){
    expr ret = Z3HandlingExprPtr(r) * Z3HandlingExprPtr(l);
    return ret;
}

z3::expr Z3Handler::Z3HandleUDiv(ExprPtr r, ExprPtr l){
    expr ret = z3::udiv(Z3HandlingExprPtr(r), Z3HandlingExprPtr(l));
    return ret;
}

z3::expr Z3Handler::Z3HandleSDiv(ExprPtr r, ExprPtr l){ // 11
    expr ret = Z3HandlingExprPtr(r) / Z3HandlingExprPtr(l) ;
    return ret;
}

z3::expr Z3Handler::Z3HandleURem(){ // not defined in Expr.h
    expr x = context_.bv_val(100, 64);
}

z3::expr Z3Handler::Z3HandleSRem(ExprPtr r, ExprPtr l){
    expr ret = z3::srem(Z3HandlingExprPtr(r), Z3HandlingExprPtr(l));
    return ret;
}

z3::expr Z3Handler::Z3HandleNeg(ExprPtr ptr){
    NegExpr *neg_expr = static_cast<NegExpr*>(ptr.get());
    if (neg_expr == NULL){
        printf("\033[47;31m Z3 Handlering ERROR : NegExpr \033[0m\n");
        throw neg_expr;
    }
    expr x = Z3HandlingExprPtr(neg_expr->getExprPtr());
    expr ret = (- x);
    return ret;
}

z3::expr Z3Handler::Z3HandleNot(ExprPtr ptr){
    NotExpr *not_expr = static_cast<NotExpr*>(ptr.get());
    if (not_expr == NULL){
        printf("\033[47;31m Z3 Handlering ERROR : NotExpr \033[0m\n");
        throw not_expr;
    }
    expr x = Z3HandlingExprPtr(not_expr->getExprPtr());
    expr ret = (~ x);
    return ret;
}

z3::expr Z3Handler::Z3HandleAnd(ExprPtr r, ExprPtr l){
    expr ret = Z3HandlingExprPtr(r) & Z3HandlingExprPtr(l);
    return ret;
}

z3::expr Z3Handler::Z3HandleOr(ExprPtr r, ExprPtr l){
    expr ret = Z3HandlingExprPtr(r) | Z3HandlingExprPtr(l);
    return ret;
}

z3::expr Z3Handler::Z3HandleXor(ExprPtr r, ExprPtr l){
    expr ret = Z3HandlingExprPtr(r) ^ Z3HandlingExprPtr(l);
    return ret;
}

z3::expr Z3Handler::Z3HandleShl(ExprPtr r, ExprPtr l){
    expr ret = z3::shl(Z3HandlingExprPtr(r), Z3HandlingExprPtr(l));
    return ret;
}

z3::expr Z3Handler::Z3HandleLShr(ExprPtr r, ExprPtr l){
    expr ret = z3::lshr(Z3HandlingExprPtr(r), Z3HandlingExprPtr(l));
    return ret;
}

z3::expr Z3Handler::Z3HandleAShr(ExprPtr r, ExprPtr l){ // 21
    expr ret = z3::ashr(Z3HandlingExprPtr(r), Z3HandlingExprPtr(l));
    return ret;
}

z3::expr Z3Handler::Z3HandleEqual(ExprPtr ptr){ //  Should have two sub-expressions?
    EqualExpr *equal_expr = static_cast<EqualExpr*>(ptr.get());
    if (equal_expr == NULL){
        printf("\033[47;31m Z3 Handlering ERROR : EqualExpr \033[0m\n");
        throw equal_expr;
    }
    expr x = Z3HandlingExprPtr(equal_expr->getExprPtr());
    return (x == 0);
}

z3::expr Z3Handler::Z3HandleDistinct(ExprPtr ptr){ // Should have two sub-expressions?
    DistinctExpr *dist_expr = static_cast<DistinctExpr*>(ptr.get());
    if (dist_expr == NULL){
        printf("\033[47;31m Z3 Handlering ERROR : DistinctExpr \033[0m\n");
        throw dist_expr;
    }
    expr x = Z3HandlingExprPtr(dist_expr->getExprPtr());
    return (x != 0);
}


// for Ult, Ule, Ugt, Uge, Slt, Sle, Sge, Sge, are they only compare with 0? rather than two sub-expressions?
// If this is the case, is it 0 always in the right side? e.g., x >0 not 0 > x?
z3::expr Z3Handler::Z3HandleUlt(ExprPtr r, ExprPtr l){
    expr x = Z3HandlingExprPtr(l);
    expr y = Z3HandlingExprPtr(r);
    expr ret = z3::ult(y, x);
    return ret;
}

z3::expr Z3Handler::Z3HandleUle(ExprPtr r, ExprPtr l){
    expr x = Z3HandlingExprPtr(l);
    expr y = Z3HandlingExprPtr(r);
    expr ret = z3::ule(y, x);
    return ret;
}

z3::expr Z3Handler::Z3HandleUgt(ExprPtr r, ExprPtr l){
    expr x = Z3HandlingExprPtr(l);
    expr y = Z3HandlingExprPtr(r);
    expr ret = z3::ugt(y, x);
    //expr ret =  (x > 0);
    return ret;
}

z3::expr Z3Handler::Z3HandleUge(ExprPtr r, ExprPtr l){
    expr x = Z3HandlingExprPtr(l);
    expr y = Z3HandlingExprPtr(r);
    expr ret = z3::uge(y, x);
    return ret;
}

z3::expr Z3Handler::Z3HandleSlt(ExprPtr r, ExprPtr l){
    expr x = Z3HandlingExprPtr(l);
    expr y = Z3HandlingExprPtr(r);
    expr ret = (y < x);
    return ret;
}

z3::expr Z3Handler::Z3HandleSle(ExprPtr r, ExprPtr l){
    expr x = Z3HandlingExprPtr(l);
    expr y = Z3HandlingExprPtr(r);
    expr ret = (y <= x);
    return ret;
}

z3::expr Z3Handler::Z3HandleSgt(ExprPtr r, ExprPtr l){
    expr x = Z3HandlingExprPtr(l);
    expr y = Z3HandlingExprPtr(r);
    expr ret = (y > x);
    return ret;
}

z3::expr Z3Handler::Z3HandleSge(ExprPtr r, ExprPtr l){ // 31
    expr x = Z3HandlingExprPtr(l);
    expr y = Z3HandlingExprPtr(r);
    expr ret = (y >= x);
    return ret;
}

z3::expr Z3Handler::Z3HandleLor(ExprPtr ptr){ // not defined in Expr.h
    expr x = context_.bv_val(100, 64);
    return x;
}

z3::expr Z3Handler::Z3HandleLAnd(ExprPtr ptr){ // not defined in Expr.h
    return context_.bv_val(100, 64);
}

z3::expr Z3Handler::Z3HandleLNot(ExprPtr ptr){
    LNotExpr *lnot_expr = static_cast<LNotExpr*>(ptr.get());
    if (lnot_expr == NULL){
        printf("\033[47;31m Z3 Handlering ERROR : LNotExpr \033[0m\n");
        throw lnot_expr;
    }
    expr x = Z3HandlingExprPtr(lnot_expr->getExprPtr());
    // expr ret = Z3HandleDistinct(lnot_expr->getExprPtr());
    expr ret = (! x); // TODO need to confirm: compare with zero?
    //expr ret = !x;
    return ret;
}

z3::expr Z3Handler::Z3HandleSignExt(ExprPtr ptr){ // not sure how to write z3 expr
    return context_.bv_val(100, 64);
}

z3::expr Z3Handler::Z3HandleZeroEXT(ExprPtr ptr){ // not sure how to write z3 expr
    return context_.bv_val(100, 64);
}

z3::expr Z3Handler::Z3HandleShrd(ExprPtr r, ExprPtr m, ExprPtr l){  // not sure how to write z3 expr
    return context_.bv_val(100, 64);
}

z3::expr Z3Handler::Z3HandleSign(ExprPtr ptr){ // not sure how to write z3 expr
    return context_.bv_val(100, 64);
}

z3::expr Z3Handler::Z3HandleNoSign(ExprPtr ptr){ // not sure how to write z3 expr
    return context_.bv_val(100, 64);
}

z3::expr Z3Handler::Z3HandleOverflow(ExprPtr ptr){ // not sure how to write z3 expr
    return context_.bv_val(100, 64);
}

z3::expr Z3Handler::Z3HandleNoOverflow(ExprPtr ptr){ // 41 // not sure how to write z3 expr
    return context_.bv_val(100, 64);
}

z3::expr Z3Handler::Z3HandleCombine(ExprPtr r, ExprPtr l){ // Is the same as Concat? This version does not consider the lsize and rsize
    expr ret = z3::concat(Z3HandlingExprPtr(r), Z3HandlingExprPtr(l));
    return ret;
}

z3::expr Z3Handler::Z3HandleExtract(ExprPtr ptr){
    ExtractExpr *extract_expr = static_cast<ExtractExpr*>(ptr.get());
    if (extract_expr == NULL){
        printf("\033[47;31m Z3 Handlering ERROR : LNotExpr \033[0m\n");
        throw extract_expr;
    }
    expr x = Z3HandlingExprPtr(extract_expr->getExprPtr());
    int s = extract_expr->getStart();
    int e = extract_expr->getEnd();
    //std::cout << "start : " << s << std::endl;
    //std::cout << "end : " << e << std::endl;
    // Finally, it should be 32-bit
    return x.extract(e*8+32 - 1, s+32); // looks different with the existing implementation
    //return x.extract(63,  32); // looks different with the existing implementation
}
