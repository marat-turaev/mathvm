#include "visitors.h"
#include "parser.h"
#include "rich_function.h"
#include <stack>

using namespace mathvm;

class BytecodeMainVisitor: public AstVisitor {
public:
    BytecodeMainVisitor(Code *code): code(code) {
        //TODO set the first 0 to actual value of the topmost function
        code->addFunction(currentFunction = new RichFunction(0, 0, 0));
        typeTokenInstruction[VT_DOUBLE][tADD] = BC_DADD;
        typeTokenInstruction[VT_INT][tADD] = BC_IADD;
        typeTokenInstruction[VT_DOUBLE][tSUB] = BC_DSUB;
        typeTokenInstruction[VT_INT][tSUB] = BC_ISUB;
        typeTokenInstruction[VT_DOUBLE][tMUL] = BC_DMUL;
        typeTokenInstruction[VT_INT][tMUL] = BC_IMUL;
        typeTokenInstruction[VT_DOUBLE][tDIV] = BC_DDIV;
        typeTokenInstruction[VT_INT][tDIV] = BC_IDIV;
    }

    virtual void visitBinaryOpNode(BinaryOpNode *node) {
        node->right()->visit(this);
        node->left()->visit(this);
        binary_math(node->kind());
    }

    virtual void visitUnaryOpNode(UnaryOpNode *node) {
        node->operand()->visit(this);
        unary_math(node->kind());
    }

    virtual void visitStringLiteralNode(StringLiteralNode *node) {
        uint16_t constantId = code->makeStringConstant(node->literal());
        currentFunction->bytecode()->addInsn(BC_SLOAD);
        currentFunction->bytecode()->addInt16(constantId);
        typesStack.push(VT_STRING);
    }

    virtual void visitDoubleLiteralNode(DoubleLiteralNode *node) {
        currentFunction->bytecode()->addInsn(BC_DLOAD);
        currentFunction->bytecode()->addInt64(node->literal());
        typesStack.push(VT_DOUBLE);
    }

    virtual void visitIntLiteralNode(IntLiteralNode *node) {
        currentFunction->bytecode()->addInsn(BC_ILOAD);
        currentFunction->bytecode()->addInt64(node->literal());
        typesStack.push(VT_INT);
    }

    virtual void visitLoadNode(LoadNode *node) {
        loadVariable(node->var());
    }

    virtual void visitStoreNode(StoreNode *node) {
        node->value()->visit(this);

        switch (node->op()) {
        case (tASSIGN): {
            storeToVariable(node->var());
            break;
        }
        case (tINCRSET): {
            loadVariable(node->var());
            binary_math(tADD);
            storeToVariable(node->var());
            break;
        }
        case (tDECRSET): {
            loadVariable(node->var());
            binary_math(tSUB);
            storeToVariable(node->var());
            break;
        }
        default: {
            assert(0);
        }
        }
    }

    virtual void visitForNode(ForNode *node) {
    }

    virtual void visitWhileNode(WhileNode *node) {
    }

    virtual void visitIfNode(IfNode *node) {
        node->ifExpr()->visit(this);

        emit(BC_ILOAD0);

        Label afterTrue(currentFunction->bytecode());
        currentFunction->bytecode()->addBranch(BC_IFICMPE, afterTrue);

        //Clear stack
        emit(BC_POP);
        emit(BC_POP);
        node->thenBlock()->visit(this);

        Label afterFalse(currentFunction->bytecode());
        if (node->elseBlock()) {
            currentFunction->bytecode()->addBranch(BC_JA, afterFalse);
        }
        afterTrue.bind(currentFunction->bytecode()->current());
        if (node->elseBlock()) {
            //Clear stack
            emit(BC_POP);
            emit(BC_POP);
            node->elseBlock()->visit(this);
            afterFalse.bind(currentFunction->bytecode()->current());
        }
    }

    virtual void visitBlockNode(BlockNode *node) {
        //variables:
        Scope::VarIterator it(node->scope());
        while (it.hasNext()) {
            AstVar *var = it.next();
            //TODO: this will override value with same name in current fucntion's scope
            currentFunction->addLocalVariable(var->name(), var->type());
        }

        //functions:
        {
            Scope::FunctionIterator it(node->scope());
            while (it.hasNext()) {
                it.next()->node()->visit(this);
            }
        }

        node->visitChildren(this);
    }

    virtual void visitFunctionNode(FunctionNode *node) {
    }

    virtual void visitReturnNode(ReturnNode *node) {
    }

    virtual void visitCallNode(CallNode *node) {
    }

    virtual void visitNativeCallNode(NativeCallNode *node) {
    }

    virtual void visitPrintNode(PrintNode *node) {
        for (uint32_t i = 0; i < node->operands(); ++i) {
            node->operandAt(i)->visit(this);
            Instruction emitted = BC_INVALID;
            switch (typesStack.top()) {
            case VT_DOUBLE: {
                emitted = BC_DPRINT;
                break;
            }
            case VT_INT: {
                emitted = BC_DPRINT;
                break;
            }
            case VT_STRING: {
                emitted = BC_SPRINT;
                break;
            }
            default: {
                assert(0);
            }
            }
            typesStack.pop();
        }
    }

private:
    Code *code;
    RichFunction *currentFunction;
    stack<VarType> typesStack;
    map<VarType, map<TokenKind, Instruction> > typeTokenInstruction;

    void emit(Instruction inst) {
        currentFunction->bytecode()->addInsn(inst);
    }

    bool convertTOS(VarType toType) {
        switch (toType) {
        case (VT_INT):  {
            switch (typesStack.top()) {
            case (VT_DOUBLE): {
                emit(BC_D2I);
                typesStack.pop();
                typesStack.push(VT_INT);
                return true;
            }
            case (VT_STRING): {
                emit(BC_S2I);
                typesStack.pop();
                typesStack.push(VT_INT);
                return true;
            }
            default: {
                return false;
            }
            }
        }
        case (VT_DOUBLE): {
            switch (typesStack.top()) {
            case (VT_INT): {
                emit(BC_I2D);
                typesStack.pop();
                typesStack.push(VT_DOUBLE);
                return true;
            }
            default: {
                return false;
            }
            }
        }
        default: {
            return false;
        }
        }
        return false;
    }

    void assertTOS(VarType type) {
        assert(typesStack.top() == type);
    }

    void loadVariable(const AstVar *astVar) {
        const string &variableName = astVar->name();
        RichFunction *actualFunction = currentFunction->lookupParentFunction(variableName);
        uint16_t variableId = actualFunction->getVariableId(variableName);
        uint16_t functionIndex = actualFunction->getIndex();

        Instruction emitted = BC_INVALID;
        switch (astVar->type()) {
        case VT_DOUBLE: {
            emitted = BC_LOADCTXDVAR;
            typesStack.push(VT_DOUBLE);
            break;
        }
        case VT_INT: {
            emitted = BC_LOADCTXIVAR;
            typesStack.push(VT_INT);
            break;
        }
        case VT_STRING: {
            emitted = BC_LOADCTXSVAR;
            typesStack.push(VT_STRING);
            break;
        }
        default: {
            assert(0);
        }
        }

        emit(emitted);
        currentFunction->bytecode()->addInt16(functionIndex);
        currentFunction->bytecode()->addInt16(variableId);
    }

    void storeToVariable(const AstVar *astVar) {
        const string &variableName = astVar->name();
        RichFunction *actualFunction = currentFunction->lookupParentFunction(variableName);
        uint16_t variableId = actualFunction->getVariableId(variableName);
        uint16_t functionIndex = actualFunction->getIndex();

        switch (astVar->type()) {
        case VT_DOUBLE: {
            convertTOS(VT_DOUBLE);
            emit(BC_STORECTXDVAR);
            break;
        }
        case VT_INT: {
            convertTOS(VT_INT);
            emit(BC_STORECTXIVAR);
            break;
        }
        case VT_STRING: {
            assertTOS(VT_STRING);
            emit(BC_STORECTXSVAR);
            break;
        }
        default: {
            assert(0);
        }
        }

        currentFunction->bytecode()->addInt16(functionIndex);
        currentFunction->bytecode()->addInt16(variableId);

        typesStack.pop();
    }

    void binary_math(TokenKind operation) {
        VarType first = typesStack.top();
        typesStack.pop();
        VarType second = typesStack.top();
        typesStack.pop();

        switch (first) {
        case (VT_DOUBLE): {
            switch (second) {
            case (VT_DOUBLE): {
                switch (operation) {
                case (tADD):
                case (tSUB):
                case (tMUL):
                case (tDIV): {
                    emit(typeTokenInstruction[VT_DOUBLE][operation]);
                    break;
                }
                default: {
                    assert(0);
                }
                }
                typesStack.push(VT_DOUBLE);
                break;
            }
            case (VT_INT): {
                emit(BC_SWAP);
                typesStack.push(VT_INT);
                convertTOS(VT_DOUBLE);
                typesStack.pop();
                emit(BC_SWAP);

                switch (operation) {
                case (tADD):
                case (tSUB):
                case (tMUL):
                case (tDIV): {
                    emit(typeTokenInstruction[VT_DOUBLE][operation]);
                    break;
                }
                default: {
                    assert(0);
                }
                }

                typesStack.push(VT_DOUBLE);
                break;
            }
            default: {
                assert(0);
            }
            }
            break;
        }
        case (VT_INT): {
            switch (second) {
            case (VT_DOUBLE): {
                typesStack.push(VT_INT);
                convertTOS(VT_DOUBLE);
                typesStack.pop();
                switch (operation) {
                case (tADD):
                case (tSUB):
                case (tMUL):
                case (tDIV): {
                    emit(typeTokenInstruction[VT_DOUBLE][operation]);
                    break;
                }
                default: {
                    assert(0);
                }
                }
                typesStack.push(VT_DOUBLE);
                break;
            }
            case (VT_INT): {
                switch (operation) {
                case (tADD):
                case (tSUB):
                case (tMUL):
                case (tDIV): {
                    emit(typeTokenInstruction[VT_INT][operation]);
                    break;
                }
                default: {
                    assert(0);
                }
                }
                typesStack.push(VT_INT);
                break;
            }
            default: {
                assert(0);
            }
            }
            break;
        }
        default: {
            assert(0);
        }
        }
    }

    void unary_math(TokenKind operation) {
        switch (typesStack.top()) {
        case (VT_DOUBLE): {
            switch (operation) {
            case (tSUB): {
                emit(BC_DLOAD0);
                typesStack.push(VT_DOUBLE);
                binary_math(tSUB);
                break;
            }
            case (tNOT): {
                emit(BC_DNEG);
                break;
            }
            default: {
                assert(0);
            }
            }
            break;
        }

        case (VT_INT): {
            switch (operation) {
            case (tSUB): {
                emit(BC_ILOAD0);
                typesStack.push(VT_INT);
                binary_math(tSUB);
                break;
            }
            case (tNOT): {
                emit(BC_INEG);
                break;
            }
            default: {
                assert(0);
            }
            }
            break;
        }

        default: {
            assert(0);
        }
        }
    }
};

class BytecodeTranslatorImpl: public Translator {
public:
    virtual Status *translate(const string &program, Code **code)  {
        Parser parser;
        Status *status = parser.parseProgram(program);
        if (status && status->isError()) {
            return status;
        }

        BytecodeMainVisitor visitor(*code);

        //TODO return Status Ok
        return 0;
    }
};

Translator *Translator::create(const string &impl) {
    // return new BytecodeTranslatorImpl();
    return 0;
}