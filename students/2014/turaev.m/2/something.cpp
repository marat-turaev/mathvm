#include "visitors.h"
#include "parser.h"

using namespace mathvm;

#define BASE_VISITOR(type) AstBaseVisitor::visit##type(node);

class ByteCodeMainVisitor : public AstBaseVisitor {
public:
    ByteCodeMainVisitor(Code *code): code(code) {
        //TODO set 0 to actual value of the topmost function
        code->addFunction(currentFunction = new BytecodeFunction(0));
    }

    virtual void visitBinaryOpNode(BinaryOpNode *node) {
    }

    virtual void visitUnaryOpNode(UnaryOpNode *node) {
    }

    virtual void visitStringLiteralNode(StringLiteralNode *node) {
        code->makeStringConstant(node->literal());
    }

    virtual void visitDoubleLiteralNode(DoubleLiteralNode *node) {
    }

    virtual void visitIntLiteralNode(IntLiteralNode *node) {
    }

    virtual void visitLoadNode(LoadNode *node) {
    }

    virtual void visitStoreNode(StoreNode *node) {
        node->value()->visit(this);
        currentFunction->_bytecode.addInsn(Instruction insn);
    }

    virtual void visitForNode(ForNode *node) {
    }

    virtual void visitWhileNode(WhileNode *node) {
    }

    virtual void visitIfNode(IfNode *node) {
    }

    virtual void visitBlockNode(BlockNode *node) {
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
    }

private:
    Code *code;
    BytecodeFunction *currentFunction;
};

class BytecodeTranslatorImpl : public Translator {
public:
    virtual Status *translate(const string &program, Code **code)  {
        Parser parser;
        Status *status = parser.parseProgram(program);
        if (status && status->isError()) {
            return status;
        }

        ByteCodeMainVisitor visitor(*code);

        //TODO return Status Ok
        return 0;
    }
};

Translator *Translator::create(const string &impl) {
    return new BytecodeTranslatorImpl();
}