#include "abstract_asm_tree.h"
#include "../ast/tree.h"
#include "intermediate_representation.h"
#include "../symbol/symbol.h"

static Instructions *instructionHead = NULL;
static Instructions *currentInstruction = NULL;
static bool mainCreated = false;

//If the context stack contains something we need to apply the instructions in the current context
//static Stack *contextStack = NULL;
static size_t currentTemporary = 0;

size_t generateInstructionsForExpression(Expression *expression, SymbolTable *symbolTable);

Instructions *newInstruction() {
    Instructions *ret = NEW(Instructions);

    ret->next = NULL;
    //ret->context = NULL;

    return ret;
}

//Helper
void appendInstructions(Instructions *new) {
    currentInstruction->next = new;

    Instructions *iter = new;

    while (iter->next != NULL) {
        iter = iter->next;
    }

    currentInstruction = iter;
}

Instructions *getLast(Instructions *current) {
    Instructions *iter = current;

    while (iter->next != NULL) {
        iter = iter->next;
    }

    return iter;
}

size_t generateInstructionsForVariableAccess(Variable *variable, SymbolTable *symbolTable) {
    switch (variable->kind) {
        case varIdK: {
            SYMBOL *symbol = getSymbol(symbolTable, variable->val.idD.id);

            //This ID should represent the variable number in the scope
            size_t uniqueVariableId = symbol->uniqueIdForScope;

            //Check how far up the scope stack we need to look for the variable
            size_t frameStackDistanceToVaraible = symbol->distanceFromRoot - symbolTable->distanceFromRoot;

            //TODO Maybe use accumulator for counting "back in memory" offset
            Instructions *fetch = newInstruction();
            fetch->kind = COMPLEX_FETCH_VARIABLE_FROM_PARENT_SCOPE_FRAME;
            fetch->val.fetchTempFromParentScope.uniqueVariableId = uniqueVariableId;
            fetch->val.fetchTempFromParentScope.distanceFromCurrentFrame = frameStackDistanceToVaraible;
            fetch->val.fetchTempFromParentScope.outputTemp = currentTemporary;
            appendInstructions(fetch);
            currentTemporary++;
            return currentTemporary - 1;
        } break;
        case arrayIndexK: {
            //TODO Array index requires multiple steps, access child first
            size_t arrayId = generateInstructionsForVariableAccess(variable->val.arrayIndexD.var, symbolTable);

            size_t arrayIndex = generateInstructionsForExpression(variable->val.arrayIndexD.idx, symbolTable);
        } break;
        case recordLookupK: {
            //TODO Record lookup works much the same as array index, evaluate the record variable itself first
            size_t recordId = generateInstructionsForVariableAccess(variable->val.recordLookupD.var, symbolTable);

            //The accessed is recordId + for every field accumulate x => sizeof(x)

        } break;
    }
}

size_t generateInstructionsForTerm(Term *term, SymbolTable *symbolTable) {

    switch (term->kind) {
        case variableK: {
            return generateInstructionsForVariableAccess(term->val.variableD.var, symbolTable);
        } break;
        case functionCallK: {
            //Give arguments on stack
            //For each expression argument, evaluate it and push it to the stack
            ExpressionList *expressionList = term->val.functionCallD.expressionList;

            while (expressionList != NULL) {

                //Evaluate
                size_t temporaryForArgument = generateInstructionsForExpression(expressionList->expression, symbolTable);

                //Then push
                Instructions *push = newInstruction();
                push->kind = INSTRUCTION_PUSH;
                push->val.tempToPush = temporaryForArgument;
                appendInstructions(push);

                expressionList = expressionList->next;
            }

            Instructions *call = newInstruction();
            call->kind = INSTRUCTION_FUNCTION_CALL;
            call->val.function = term->val.functionCallD.functionId;
            appendInstructions(call);

            //We have the return value on the stack
            Instructions *pop = newInstruction();
            pop->kind = INSTRUCTION_POP;
            pop->val.tempToPopInto = currentTemporary;
            appendInstructions(call);
            currentTemporary++;
            return currentTemporary - 1;
        } break;
        case parenthesesK: {
            return generateInstructionsForExpression(term->val.parenthesesD.expression, symbolTable);
        } break;
        case negateK: {
            size_t result = generateInstructionsForTerm(term->val.negateD.term, symbolTable);

            Instructions *neg = newInstruction();
            neg->kind = INSTRUCTION_NEGATE;
            neg->val.tempToNegate = result;
            appendInstructions(neg);

            return result;
        } break;
        case absK: {
            //TODO DO this manually or MMX SSSE3 PABSD
        } break;
        case numK: {
            Instructions *num = newInstruction();
            num->kind = INSTRUCTION_CONST;
            num->val.constant.value = term->val.numD.num;
            num->val.constant.temp = currentTemporary;
            appendInstructions(num);
            currentTemporary++;
            return currentTemporary - 1;
        } break;
        case trueK: {
            Instructions *num = newInstruction();
            num->kind = INSTRUCTION_CONST;
            num->val.constant.value = 1;
            num->val.constant.temp = currentTemporary;
            appendInstructions(num);
            currentTemporary++;
            return currentTemporary - 1;
        } break;
        case falseK: {
            Instructions *num = newInstruction();
            num->kind = INSTRUCTION_CONST;
            num->val.constant.value = 0;
            num->val.constant.temp = currentTemporary;
            appendInstructions(num);
            currentTemporary++;
            return currentTemporary - 1;
        } break;
        case nullK: {
            //TODO point at 0
        } break;
        case lambdaK: {
            //TODO
        } break;
        case classDowncastk: {
            //TODO
        } break;
    }
}

//size_t is the resulting temporary
size_t generateInstructionsForExpression(Expression *expression, SymbolTable *symbolTable) {
    switch (expression->kind) {
        case opK: {
            size_t lhsTemp = generateInstructionsForExpression(expression->val.op.left, symbolTable);
            size_t rhsTemp = generateInstructionsForExpression(expression->val.op.right, symbolTable);

            switch (expression->val.op.operator->kind) {
                case opMultK: {
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MUL;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
                case opDivK: {
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_DIV;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
                case opPlusK: {
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_ADD;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
                case opMinusK: {
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
                case opInequalityK: {
                    //subtract
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
                case opEqualityK: {
                    //subtract and use 1-x trick to invert
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);

                    Instructions *constant = newInstruction();
                    constant->kind = INSTRUCTION_CONST;
                    constant->val.constant.value = 1;
                    constant->val.constant.temp = currentTemporary;
                    appendInstructions(constant);
                    currentTemporary++;

                    Instructions *trick = newInstruction();
                    trick->kind = INSTRUCTION_MINUS;
                    trick->val.arithmetic2.source = currentTemporary - 1; // For the 1 constant
                    trick->val.arithmetic2.dest = rhsTemp; // For the resulting subtraction
                    appendInstructions(trick);

                    return rhsTemp;
                } break;
                case opGreaterK: {
                    //TODO Use smart x86 instruction for this
                    //x > y -> lhs > rhs
                    //subtract x from y and check if the result is > 0 later
                    //also constrain to boolean
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);

                    Instructions *constrain = newInstruction();
                    constrain->kind = COMPLEX_CONSTRAIN_BOOLEAN;
                    constrain->val.tempToConstrain = rhsTemp;
                    appendInstructions(constrain);
                    return rhsTemp;
                } break;
                case opLessK: {
                    //TODO Use smart x86 instruction for this
                    //Same as above, but lhs and rhs flipped
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic2.source = rhsTemp;
                    instruction->val.arithmetic2.dest = lhsTemp;
                    appendInstructions(instruction);

                    Instructions *constrain = newInstruction();
                    constrain->kind = COMPLEX_CONSTRAIN_BOOLEAN;
                    constrain->val.tempToConstrain = lhsTemp;
                    appendInstructions(constrain);

                    return lhsTemp;
                } break;
                case opGeqK: {
                    //TODO Use smart x86 instruction for this
                    //Can be hacked by using greater but adding a constant of one
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);

                    Instructions *constant = newInstruction();
                    constant->kind = INSTRUCTION_CONST;
                    constant->val.constant.value = 1;
                    constant->val.constant.temp = currentTemporary;
                    appendInstructions(constant);
                    currentTemporary++;

                    Instructions *trick = newInstruction();
                    trick->kind = INSTRUCTION_ADD;
                    trick->val.arithmetic2.source = currentTemporary - 1; // For the 1 constant
                    trick->val.arithmetic2.dest = rhsTemp; // For the resulting subtraction
                    appendInstructions(trick);

                    Instructions *constrain = newInstruction();
                    constrain->kind = COMPLEX_CONSTRAIN_BOOLEAN;
                    constrain->val.tempToConstrain = rhsTemp;
                    appendInstructions(constrain);

                    return rhsTemp;
                } break;
                case opLeqK: {
                    //TODO Use smart x86 instruction for this
                    //Same as above, but lhs and rhs flipped
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic2.source = rhsTemp;
                    instruction->val.arithmetic2.dest = lhsTemp;
                    appendInstructions(instruction);

                    Instructions *constant = newInstruction();
                    constant->kind = INSTRUCTION_CONST;
                    constant->val.constant.value = 1;
                    constant->val.constant.temp = currentTemporary;
                    appendInstructions(constant);
                    currentTemporary++;

                    Instructions *trick = newInstruction();
                    trick->kind = INSTRUCTION_ADD;
                    trick->val.arithmetic2.source = currentTemporary - 1; // For the 1 constant
                    trick->val.arithmetic2.dest = lhsTemp; // For the resulting subtraction
                    appendInstructions(trick);

                    Instructions *constrain = newInstruction();
                    constrain->kind = COMPLEX_CONSTRAIN_BOOLEAN;
                    constrain->val.tempToConstrain = lhsTemp;
                    appendInstructions(constrain);

                    return lhsTemp;
                } break;
                case opAndK: {
                    //and lhs and rhs
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_AND;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
                case opOrK: {
                    //and lhs and rhs
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_OR;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
            }
        }
            break;
        case termK: {
            return generateInstructionsForTerm(expression->val.termD.term, symbolTable);
        }
            break;
    }
}

void generateInstructionTreeForStatement(Statement *statement) {


    switch (statement->kind) {
        case statReturnK: {
            Context *currentContext = NEW(Context);
            currentContext->kind = ACCUMULATE;
            currentContext->val.temp = currentTemporary;

            //push(contextStack, currentContext);

            size_t resultingTemporary = generateInstructionsForExpression(statement->val.returnD.exp, statement->symbolTable);

            Instructions *instructions = newInstruction();
            instructions->kind = INSTRUCTION_RETURN;
            instructions->val.tempToReturn = resultingTemporary;
            appendInstructions(instructions);

            //pop(contextStack);

            currentTemporary++;
        }
            break;
        case statWriteK: {
            Instructions *instructions = newInstruction();
            instructions->kind = INSTRUCTION_WRITE;
            instructions->val.tempToWrite = generateInstructionsForExpression(statement->val.writeD.exp, statement->symbolTable);
            appendInstructions(instructions);
        }
            break;
        case statAllocateK: {
            Instructions *num = newInstruction();
            num->kind = INSTRUCTION_CONST;
            num->val.constant.value = 1;
            num->val.constant.temp = currentTemporary;
            appendInstructions(num);
            currentTemporary++;

            size_t accessTemp = generateInstructionsForVariableAccess(statement->val.allocateD.var, statement->symbolTable);
            Type *type = unwrapVariable(statement->val.allocateD.var, statement->symbolTable);
            Instructions *ret = newInstruction();
            ret->kind = COMPLEX_ALLOCATE;
            ret->val.allocate.timesTemp = currentTemporary - 1;
            ret->val.allocate.accessTemp = accessTemp;
            ret->val.allocate.tpe = type;
            appendInstructions(ret);
        } break;
        case statAllocateLenK: {
            size_t accessTemp = generateInstructionsForVariableAccess(statement->val.allocateLenD.var, statement->symbolTable);
            Type *type = unwrapVariable(statement->val.allocateLenD.var, statement->symbolTable);
            size_t times = generateInstructionsForExpression(statement->val.allocateLenD.len, statement->symbolTable);
            Instructions *ret = newInstruction();
            ret->kind = COMPLEX_ALLOCATE;
            ret->val.allocate.timesTemp = times;
            ret->val.allocate.accessTemp = accessTemp;
            ret->val.allocate.tpe = type;
            appendInstructions(ret);
        } break;
        case statIfK:
            //TODO
            break;
        case statIfElK:
            //TODO
            break;
        case statWhileK:
            //TODO
            break;
        case stmListK:
            //TODO
            break;
        case assignmentK: {
            //For now we simply tell that temporary must be moved back to variable number x
            //Fetch variable
            SYMBOL *symbol = getSymbol(statement->symbolTable, statement->val.assignmentD.var->val.idD.id);

            size_t expressionTemp = generateInstructionsForExpression(statement->val.assignmentD.exp, statement->symbolTable);

            //We need to move the n'th variable into a register and then move the value into the register's de-referenced pointer
            Instructions *load = newInstruction();
            load->kind = COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK;
            load->val.ptrLoad.var = symbol;
            load->val.ptrLoad.temporary = currentTemporary;
            currentTemporary++;
            appendInstructions(load);

            Instructions *save = newInstruction();
            save->kind = COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER;
            save->val.ptrSave.tempPtr = currentTemporary - 1;
            save->val.ptrSave.tempValue = expressionTemp;
            appendInstructions(save);
        } break;
    }
}

void generateInstructionTreeForDeclaration(Declaration *declaration) {

    switch (declaration->kind) {
        case declVarK: {
            SYMBOL *symbol = getSymbol(declaration->symbolTable, declaration->val.varD.id);
            Instructions *ret = newInstruction();
            ret->kind = INSTRUCTION_VAR;
            ret->val.var = symbol;

            appendInstructions(ret);
        }
        break;
        case declVarsK: {
            //For current var
            Declaration *thisVar = declaration->val.varsD.var;

            SYMBOL *symbol = getSymbol(declaration->symbolTable, thisVar->val.varD.id);
            Instructions *ret = newInstruction();
            ret->kind = INSTRUCTION_VAR;
            ret->val.var = symbol;

            if (declaration->val.varsD.next == NULL) {
                appendInstructions(ret);
            } else {
                generateInstructionTreeForDeclaration(declaration->val.varsD.next);
            }
        }
        break;
        case declTypeK:
            //SKIP
            break;
        case declFuncK: {
            //For function label
            Instructions *label = newInstruction();
            label->val.label = declaration->val.functionD.function->head->indentifier;
            label->kind = INSTRUCTION_FUNCTION_LABEL;

            //Readjust arg
            appendInstructions(label);

            //For args we generate metadata for later (maybe this is useless, who knows?)
            VarDelList *declarationList = declaration->val.functionD.function->head->declarationList;
            size_t counter = 0;

            while (declarationList != NULL) {
                //Create arg instr for this argument
                Instructions *arg = newInstruction();
                arg->kind = METADATA_FUNCTION_ARGUMENT;
                arg->val.argNum = counter;
                appendInstructions(arg);

                declarationList = declarationList->next;
                counter++;
            }

            generateInstructionTree(declaration->val.functionD.function->body);

            Instructions *tail = newInstruction();
            tail->kind = INSTRUCTION_FUNCTION_END;
            tail->val.label = declaration->val.functionD.function->tail->indentifier;
            appendInstructions(tail);
        }
            break;
        case declValK: {
            SYMBOL *symbol = getSymbol(declaration->symbolTable, declaration->val.valD.id);
            Instructions *ret = newInstruction();
            ret->kind = INSTRUCTION_VAR;
            ret->val.var = symbol;
            appendInstructions(ret);
            //TODO RHS evaluation needed for this
        }
            break;
        case declClassK:
            //TODO
            break;
    }
}

Instructions* generateInstructionTree(Body *body) {
    //Save temporary counter
    bool createMain = false;

    if (!mainCreated) {
        createMain = true;
        mainCreated = true;
    }

    size_t restoreTemporary = currentTemporary;

    DeclarationList *declarationList = body->declarationList;
    StatementList *statementList = body->statementList;
    Instructions *head = newInstruction();
    head->kind = METADATA_BEGIN_BODY_BLOCK;

    if (instructionHead == NULL) {
        instructionHead = head;
        currentInstruction = instructionHead;
    }

    //if (contextStack == NULL) {
      //  contextStack = initStack();
    //}

    //Functions will take tail and return the new tail, head must be pre-inited

    while (declarationList != NULL) {
        generateInstructionTreeForDeclaration(declarationList->declaration);
        declarationList = declarationList->next;
    }


    if (createMain) {
        Instructions *declsEnd = newInstruction();
        declsEnd->kind = METADATA_CREATE_MAIN;
        appendInstructions(declsEnd);
    }

    while (statementList != NULL) {
        generateInstructionTreeForStatement(statementList->statement);
        statementList = statementList->next;
    }

    currentInstruction->next = newInstruction();
    currentInstruction = currentInstruction->next;
    currentInstruction->kind = METADATA_END_BODY_BLOCK;

    //Restore temporary counter
    currentTemporary = restoreTemporary;

    return instructionHead;
}