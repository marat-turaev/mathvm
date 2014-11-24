#ifndef INTERPRETER_CODE_IMPL_H
#define INTERPRETER_CODE_IMPL_H

#include "mathvm.h"
#include <stack>
#include <iostream>

using namespace std;

namespace mathvm {
class InterpreterCodeImpl: public Code {
    map<Instruction, uint32_t> instrSize;
    stack<uint16_t> stringStack;
    stack<int64_t> intStack;
    stack<double> doubleStack;

    void setSizes() {
#define SET_SIZE(b, d, l) instrSize[BC_##b] = l;
        FOR_BYTECODES(SET_SIZE);
#undef SET_SIZE
    }

public:
    virtual Status *execute(vector<Var *> &vars) {
        setSizes();

        return Status::Ok();
    }

    void executeFunction(BytecodeFunction *function) {
        uint32_t ip = 0;
        Bytecode *bytecode = function->bytecode();
        while (ip != bytecode->length()) {
            Instruction current = bytecode->getInsn(ip);

            switch (current) {
                //TODO assert that stack same size before and after execution
                case BC_DLOAD: {
                    doubleStack.push(bytecode->getDouble(ip + 1));
                    break;
                }
                case BC_ILOAD: {
                    intStack.push(bytecode->getInt64(ip + 1));
                    break;
                }
                case BC_SLOAD: {
                    stringStack.push(bytecode->getUInt16(ip + 1));
                    break;
                }
                case BC_ILOAD0: {
                    intStack.push(0);
                    break;
                }
                case BC_DADD: {
                    double left = doubleStack.top();
                    doubleStack.pop();
                    double right = doubleStack.top();
                    doubleStack.pop();
                    doubleStack.push(left + right);
                    break;
                }
                case BC_IADD: {
                    int64_t left = intStack.top();
                    intStack.pop();
                    int64_t right = intStack.top();
                    intStack.pop();
                    intStack.push(left + right);
                    break;
                }
                case BC_DSUB: {
                    double left = doubleStack.top();
                    doubleStack.pop();
                    double right = doubleStack.top();
                    doubleStack.pop();
                    doubleStack.push(left - right);
                    break;
                }
                case BC_ISUB: {
                    int64_t left = intStack.top();
                    intStack.pop();
                    int64_t right = intStack.top();
                    intStack.pop();
                    intStack.push(left - right);
                    break;
                }
                case BC_DMUL: {
                    double left = doubleStack.top();
                    doubleStack.pop();
                    double right = doubleStack.top();
                    doubleStack.pop();
                    doubleStack.push(left * right);
                    break;
                }
                case BC_IMUL: {
                    int64_t left = intStack.top();
                    intStack.pop();
                    int64_t right = intStack.top();
                    intStack.pop();
                    intStack.push(left * right);
                    break;
                }
                case BC_DDIV: {
                    double left = doubleStack.top();
                    doubleStack.pop();
                    double right = doubleStack.top();
                    doubleStack.pop();
                    doubleStack.push(left / right);
                    break;
                }
                case BC_IDIV: {
                    int64_t left = intStack.top();
                    intStack.pop();
                    int64_t right = intStack.top();
                    intStack.pop();
                    intStack.push(left / right);
                    break;
                }
                case BC_IMOD: {
                    int64_t left = intStack.top();
                    intStack.pop();
                    int64_t right = intStack.top();
                    intStack.pop();
                    intStack.push(left % right);
                    break;
                }
                case BC_DNEG: {
                    double left = doubleStack.top();
                    doubleStack.pop();
                    doubleStack.push(-left);
                    break;
                }
                case BC_INEG: {
                    int64_t left = intStack.top();
                    intStack.pop();
                    intStack.push(-left);
                    break;
                }
                case BC_IAOR: {
                    int64_t left = intStack.top();
                    intStack.pop();
                    int64_t right = intStack.top();
                    intStack.pop();
                    intStack.push(left | right);
                    break;
                }
                case BC_IAAND: {
                    int64_t left = intStack.top();
                    intStack.pop();
                    int64_t right = intStack.top();
                    intStack.pop();
                    intStack.push(left & right);
                    break;
                }
                case BC_IAXOR: {
                    int64_t left = intStack.top();
                    intStack.pop();
                    int64_t right = intStack.top();
                    intStack.pop();
                    intStack.push(left ^ right);
                    break;
                }
                case BC_IPRINT: {
                    int64_t arg = intStack.top();
                    intStack.pop();
                    cout << arg;
                    break;
                }
                case BC_DPRINT: {
                    double arg = doubleStack.top();
                    doubleStack.pop();
                    cout << arg;
                    break;
                }
                case BC_SPRINT: {
                    uint16_t arg = stringStack.top();
                    stringStack.pop();
                    cout << constantById(arg);
                    break;
                }
                case BC_I2D: {
                    assert(0);
                    break;
                }
                case BC_D2I: {
                    assert(0);
                    break;
                }
                case BC_S2I: {
                    assert(0);
                    break;
                }
                case BC_SWAP: {
                    assert(0);
                    break;
                }
                case BC_POP: {
                    assert(0);
                    break;
                }
                case BC_LOADDVAR0: {
                    assert(0);
                    break;
                }
                case BC_LOADDVAR1: {
                    assert(0);
                    break;
                }
                case BC_LOADDVAR2: {
                    assert(0);
                    break;
                }
                case BC_LOADDVAR3: {
                    assert(0);
                    break;
                }
                case BC_LOADIVAR0: {
                    assert(0);
                    break;
                }
                case BC_LOADIVAR1: {
                    assert(0);
                    break;
                }
                case BC_LOADIVAR2: {
                    assert(0);
                    break;
                }
                case BC_LOADIVAR3: {
                    assert(0);
                    break;
                }
                case BC_LOADSVAR0: {
                    assert(0);
                    break;
                }
                case BC_LOADSVAR1: {
                    assert(0);
                    break;
                }
                case BC_LOADSVAR2: {
                    assert(0);
                    break;
                }
                case BC_LOADSVAR3: {
                    assert(0);
                    break;
                }
                case BC_STOREDVAR0: {
                    assert(0);
                    break;
                }
                case BC_STOREDVAR1: {
                    assert(0);
                    break;
                }
                case BC_STOREDVAR2: {
                    assert(0);
                    break;
                }
                case BC_STOREDVAR3: {
                    assert(0);
                    break;
                }
                case BC_STOREIVAR0: {
                    assert(0);
                    break;
                }
                case BC_STOREIVAR1: {
                    assert(0);
                    break;
                }
                case BC_STOREIVAR2: {
                    assert(0);
                    break;
                }
                case BC_STOREIVAR3: {
                    assert(0);
                    break;
                }
                case BC_STORESVAR0: {
                    assert(0);
                    break;
                }
                case BC_STORESVAR1: {
                    assert(0);
                    break;
                }
                case BC_STORESVAR2: {
                    assert(0);
                    break;
                }
                case BC_STORESVAR3: {
                    assert(0);
                    break;
                }
                case BC_LOADDVAR: {
                    assert(0);
                    break;
                }
                case BC_LOADIVAR: {
                    assert(0);
                    break;
                }
                case BC_LOADSVAR: {
                    assert(0);
                    break;
                }
                case BC_STOREDVAR: {
                    assert(0);
                    break;
                }
                case BC_STOREIVAR: {
                    assert(0);
                    break;
                }
                case BC_STORESVAR: {
                    assert(0);
                    break;
                }
                case BC_LOADCTXDVAR: {
                    assert(0);
                    break;
                }
                case BC_LOADCTXIVAR: {
                    assert(0);
                    break;
                }
                case BC_LOADCTXSVAR: {
                    assert(0);
                    break;
                }
                case BC_STORECTXDVAR: {
                    assert(0);
                    break;
                }
                case BC_STORECTXIVAR: {
                    assert(0);
                    break;
                }
                case BC_STORECTXSVAR: {
                    assert(0);
                    break;
                }
                case BC_DCMP: {
                    assert(0);
                    break;
                }
                case BC_ICMP: {
                    assert(0);
                    break;
                }
                case BC_JA: {
                    assert(0);
                    break;
                }
                case BC_IFICMPNE: {
                    assert(0);
                    break;
                }
                case BC_IFICMPE: {
                    assert(0);
                    break;
                }
                case BC_IFICMPG: {
                    assert(0);
                    break;
                }
                case BC_IFICMPGE: {
                    assert(0);
                    break;
                }
                case BC_IFICMPL: {
                    assert(0);
                    break;
                }
                case BC_IFICMPLE: {
                    assert(0);
                    break;
                }
                case BC_DUMP: {
                    assert(0);
                    break;
                }
                case BC_STOP: {
                    assert(0);
                    break;
                }
                case BC_CALL: {
                    assert(0);
                    break;
                }
                case BC_CALLNATIVE: {
                    assert(0);
                    break;
                }
                case BC_RETURN: {
                    assert(0);
                    break;
                }
                case BC_BREAK: {
                    assert(0);
                    break;
                }

                default: {
                    assert(0);
                }
            }
            ip += instrSize[current];
        }
    }
};
}

#endif