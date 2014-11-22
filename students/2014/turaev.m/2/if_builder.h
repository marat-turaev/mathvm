#ifndef IF_BUILDER_H
#define IF_BUILDER_H

#include "bytecode_visitor.cpp"
#include "mathvm.h"

namespace {
class IfStep;
class ThenStep;
class ElseStep;
}

class IfBuilder {
protected:
    BytecodeMainVisitor *_visitor;
    Bytecode *_bytecode;
public:
    IfBuilder(BytecodeMainVisitor *visitor, Bytecode *bytecode): _visitor(visitor), _bytecode(bytecode) { }
    IfStep If(AstNode *node);
};

namespace {
class IfStep: public IfBuilder {
private:
    Label _afterTrueLabel;
public:
    IfStep(Label afterTrueLabel, BytecodeMainVisitor *visitor, Bytecode *bytecode): IfBuilder(visitor, bytecode), _afterTrueLabel(afterTrueLabel) {};
    ThenStep Then(AstNode *);
};
class ThenStep: public IfBuilder {
private:
    Label _afterTrueLabel;
    Label _afterFalseLabel;
public:
    ThenStep(Label afterFalseLabel, Label afterTrueLabel, BytecodeMainVisitor *visitor, Bytecode *bytecode): IfBuilder(visitor, bytecode), _afterTrueLabel(afterTrueLabel), _afterFalseLabel(afterFalseLabel) {};
    ElseStep Else(AstNode *);
    void Done();
};
class ElseStep: public IfBuilder {
private:
    Label _afterTrueLabel;
    Label _afterFalseLabel;
public:
    ElseStep(Label afterFalseLabel, Label afterTrueLabel, BytecodeMainVisitor *visitor, Bytecode *bytecode): IfBuilder(visitor, bytecode), _afterTrueLabel(afterTrueLabel), _afterFalseLabel(afterFalseLabel) {};
    void Done();
};
}

IfStep IfBuilder::If(AstNode *node)  {
    node->visit(_visitor);
    _bytecode->addInsn(BC_ILOAD0);
    Label afterTrue(_bytecode);
    _bytecode->addBranch(BC_IFICMPE, afterTrue);
    return IfStep(afterTrue, _visitor, _bytecode);
}

ThenStep IfStep::Then(AstNode *node) {
    node->visit(_visitor);
    Label afterFalse(_bytecode);
    return ThenStep(afterFalse, _afterTrueLabel, _visitor, _bytecode);
}

ElseStep ThenStep::Else(AstNode *node) {
    _bytecode->addBranch(BC_JA, _afterFalseLabel);
    _afterTrueLabel.bind(_bytecode->current());
    node->visit(_visitor);
    _afterFalseLabel.bind(_bytecode->current());
    return ElseStep(_afterFalseLabel, _afterTrueLabel, _visitor, _bytecode);
}

void ThenStep::Done() {
    _afterTrueLabel.bind(_bytecode->current());
}

#endif