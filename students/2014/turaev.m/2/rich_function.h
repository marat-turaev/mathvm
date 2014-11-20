#ifndef RICH_FUNCTION_H
#define RICH_FUNCTION_H

#include "mathvm.h"
#include "ast.h"

using namespace std;
using namespace mathvm;

class RichFunction : public BytecodeFunction {
    map<string, uint16_t> _local_variables;
    uint16_t _variable_id;
public:
    RichFunction(AstFunction *function): BytecodeFunction(function) {
        for (uint16_t i = 0; i < parametersNumber(); ++i) {
            _local_variables[parameterName(i)] = _variable_id++;
        }
    }

    RichFunction *lookupParentFunction(const string &name) {
        return
            _local_variables.find(name) == _local_variables.end()
            ? _parent->lookupParentFunction(name)
            : this;
    }

    uint16_t getIndex() {
        return _index;
    }

    uint16_t getVariableId(const string &name) {
        return _local_variables[name].second;
    }

    void addLocalVariable(const string &name, VarType type) {
        _local_variables[name] = std::make_pair(type, _variable_id++);
    }

    virtual ~RichFunction() {

    }
};

#endif