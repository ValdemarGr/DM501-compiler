#include "abstract_asm_tree.h"
#include "intermediate_representation.h"
#include "../ast/tree.h"
#include "../symbol/symbol.h"


static Instructions *instructionHead = NULL;
static Instructions *currentInstruction = NULL;
static bool mainCreated = false;
static size_t ifCounter = 0;
static size_t whileCounter = 0;

//If the context stack contains something we need to apply the instructions in the current context
//static Stack *contextStack = NULL;
static size_t currentTemporary = 0;

size_t generateInstructionsForExpression(Expression *expression, SymbolTable *symbolTable);

//Helper
void appendInstructions(Instructions *new) {
    currentInstruction->next = new;
    new->previous = currentInstruction;

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
            size_t frameStackDistanceToVariable = symbolTable->distanceFromRoot - symbol->distanceFromRoot;

            Type *unwrapped = unwrapTypedef(symbol->value->val.typeD.tpe, symbolTable);

            size_t offset = 0;

            if (unwrapped->kind == typeIntK || unwrapped->kind == typeBoolK) {
                offset = getSizeForType(unwrapped);
            } else {
                offset = POINTER_SIZE;
            }

            if (frameStackDistanceToVariable == 0) {
                Instructions *ptrAccess = newInstruction();
                ptrAccess->kind = COMPLEX_MOVE_TEMPORARY_FROM_STACK;
                ptrAccess->val.tempFromStack.offset = offset * (symbol->uniqueIdForScope + 1);
                ptrAccess->val.tempFromStack.inputTemp = currentTemporary;
                appendInstructions(ptrAccess);
                currentTemporary++;
                return currentTemporary - 1;
            } else {
                Instructions *ptrAccess = newInstruction();
                ptrAccess->kind = COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE;
                ptrAccess->val.tempFromStackScope.offset = offset * (symbol->uniqueIdForScope + 1);
                ptrAccess->val.tempFromStackScope.inputTemp = currentTemporary;
                ptrAccess->val.tempFromStackScope.scopeToFindFrame = symbol->distanceFromRoot;
                appendInstructions(ptrAccess);
                currentTemporary++;
                return currentTemporary - 1;
            }
        } break;
        case arrayIndexK: {
            size_t accessTemp = generateInstructionsForVariableAccess(variable->val.arrayIndexD.var, symbolTable);

            size_t exprTemp = generateInstructionsForExpression(variable->val.arrayIndexD.idx, symbolTable);

            Type *arrayOfType = unwrapVariable(variable->val.arrayIndexD.var, symbolTable)->val.arrayType.type;

            size_t sizeAccumulator = 0;

            if (arrayOfType->kind == typeIntK || arrayOfType->kind == typeBoolK) {
                sizeAccumulator = getSizeForType(arrayOfType);
            } else {
                sizeAccumulator = POINTER_SIZE;
            }

            Instructions *tpeConst = newInstruction();
            tpeConst->kind = INSTRUCTION_CONST;
            tpeConst->val.constant.temp = currentTemporary;
            tpeConst->val.constant.value = (int)sizeAccumulator;
            currentTemporary++;
            appendInstructions(tpeConst);

            Instructions *mulOffset = newInstruction();
            mulOffset->kind = INSTRUCTION_MUL;
            mulOffset->val.arithmetic2.source = currentTemporary - 1;
            mulOffset->val.arithmetic2.dest = exprTemp;
            appendInstructions(mulOffset);


            Instructions *ptrAccess = newInstruction();
            ptrAccess->kind = COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET;
            ptrAccess->val.dereferenceOffset.ptrTemp = accessTemp;
            ptrAccess->val.dereferenceOffset.offsetTemp = exprTemp;
            appendInstructions(ptrAccess);
            return accessTemp;
        } break;
        case recordLookupK: {
            size_t accessTemp = generateInstructionsForVariableAccess(variable->val.recordLookupD.var, symbolTable);

            Type *unwrappedType = unwrapVariable(variable->val.recordLookupD.var, symbolTable);
            VarDelList *varDelList = unwrappedType->val.recordType.types;
            size_t sizeAccumulator = 0;

            //Todo classes

            while (strcmp(varDelList->identifier, variable->val.recordLookupD.id) != 0) {
                Type *unwrapped = unwrapTypedef(varDelList->type, symbolTable);
                //If int or bool we store them as primitives, else pointers
                if (unwrapped->kind == typeIntK || unwrapped->kind == typeBoolK) {
                    sizeAccumulator = sizeAccumulator + getSizeForType(varDelList->type);
                } else {
                    sizeAccumulator = sizeAccumulator + POINTER_SIZE;
                }

                varDelList = varDelList->next;
            }

            Instructions *constOffset = newInstruction();
            constOffset->kind = INSTRUCTION_CONST;
            constOffset->val.constant.value = (int)sizeAccumulator;
            constOffset->val.constant.temp = currentTemporary;
            appendInstructions(constOffset);
            currentTemporary++;

            Instructions *ptrAccess = newInstruction();
            ptrAccess->kind = COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET;
            ptrAccess->val.dereferenceOffset.ptrTemp = accessTemp;
            ptrAccess->val.dereferenceOffset.offsetTemp = currentTemporary - 1;
            appendInstructions(ptrAccess);
            return accessTemp;
        } break;
    }
}

void generateInstructionsForVariableSave(Variable *variable, SymbolTable *symbolTable, size_t tempToSave) {
    switch (variable->kind) {
        case varIdK: {
            SYMBOL *symbol = getSymbol(symbolTable, variable->val.idD.id);

            //Check how far up the scope stack we need to look for the variable
            size_t frameStackDistanceToVariable = symbolTable->distanceFromRoot - symbol->distanceFromRoot;

            Type *unwrapped = unwrapTypedef(symbol->value->val.typeD.tpe, symbolTable);

            size_t offset = 0;

            if (unwrapped->kind == typeIntK || unwrapped->kind == typeBoolK) {
                offset = getSizeForType(unwrapped);
            } else {
                offset = POINTER_SIZE;
            }

            if (frameStackDistanceToVariable == 0) {
                Instructions *ptrAccess = newInstruction();
                ptrAccess->kind = COMPLEX_MOVE_TEMPORARY_INTO_STACK;
                ptrAccess->val.tempIntoStack.offset = offset * (symbol->uniqueIdForScope + 1);
                ptrAccess->val.tempIntoStack.tempToMove = tempToSave;
                appendInstructions(ptrAccess);
            } else {
                Instructions *ptrAccess = newInstruction();
                ptrAccess->kind = COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE;
                ptrAccess->val.tempIntoStackScope.offset = offset * (symbol->uniqueIdForScope + 1);
                ptrAccess->val.tempIntoStackScope.tempToMove = tempToSave;
                ptrAccess->val.tempIntoStackScope.scopeToFindFrame = symbol->distanceFromRoot;
                appendInstructions(ptrAccess);
            }
        } break;
        case arrayIndexK: {
            size_t accessTemp = generateInstructionsForVariableAccess(variable->val.arrayIndexD.var, symbolTable);

            size_t exprTemp = generateInstructionsForExpression(variable->val.arrayIndexD.idx, symbolTable);

            Type *arrayOfType = unwrapVariable(variable->val.arrayIndexD.var, symbolTable)->val.arrayType.type;

            size_t sizeAccumulator = 0;

            if (arrayOfType->kind == typeIntK || arrayOfType->kind == typeBoolK) {
                sizeAccumulator = getSizeForType(arrayOfType);
            } else {
                sizeAccumulator = POINTER_SIZE;
            }

            Instructions *tpeConst = newInstruction();
            tpeConst->kind = INSTRUCTION_CONST;
            tpeConst->val.constant.temp = currentTemporary;
            tpeConst->val.constant.value = (int)sizeAccumulator;
            currentTemporary++;
            appendInstructions(tpeConst);

            Instructions *mulOffset = newInstruction();
            mulOffset->kind = INSTRUCTION_MUL;
            mulOffset->val.arithmetic2.source = currentTemporary - 1;
            mulOffset->val.arithmetic2.dest = exprTemp;
            appendInstructions(mulOffset);


            Instructions *ptrAccess = newInstruction();
            ptrAccess->kind = INSTRUCTION_MOVE_TO_OFFSET;
            ptrAccess->val.moveToOffset.ptrTemp = accessTemp;
            ptrAccess->val.moveToOffset.offsetTemp = exprTemp;
            ptrAccess->val.moveToOffset.tempToMove = tempToSave;
            appendInstructions(ptrAccess);
        } break;
        case recordLookupK: {
            size_t accessTemp = generateInstructionsForVariableAccess(variable->val.recordLookupD.var, symbolTable);

            Type *unwrappedType = unwrapVariable(variable->val.recordLookupD.var, symbolTable);
            VarDelList *varDelList = unwrappedType->val.recordType.types;
            size_t sizeAccumulator = 0;

            //Todo classes

            while (strcmp(varDelList->identifier, variable->val.recordLookupD.id) != 0) {
                Type *unwrapped = unwrapTypedef(varDelList->type, symbolTable);
                //If int or bool we store them as primitives, else pointers
                if (unwrapped->kind == typeIntK || unwrapped->kind == typeBoolK) {
                    sizeAccumulator = sizeAccumulator + getSizeForType(varDelList->type);
                } else {
                    sizeAccumulator = sizeAccumulator + POINTER_SIZE;
                }

                varDelList = varDelList->next;
            }

            Instructions *constOffset = newInstruction();
            constOffset->kind = INSTRUCTION_CONST;
            constOffset->val.constant.value = (int)sizeAccumulator;
            constOffset->val.constant.temp = currentTemporary;
            appendInstructions(constOffset);
            currentTemporary++;

            Instructions *ptrAccess = newInstruction();
            ptrAccess->kind = INSTRUCTION_MOVE_TO_OFFSET;
            ptrAccess->val.moveToOffset.ptrTemp = accessTemp;
            ptrAccess->val.moveToOffset.offsetTemp = currentTemporary - 1;
            ptrAccess->val.moveToOffset.tempToMove = tempToSave;
            appendInstructions(ptrAccess);
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
            SYMBOL *symbol = getSymbol(symbolTable, term->val.functionCallD.functionId);

            if (symbol->distanceFromRoot == symbolTable->distanceFromRoot + 1) {
                //We need to save the current static link pointer
                Instructions *push = newInstruction();
                push->kind = COMPLEX_SAVE_STATIC_LINK;
                push->val.pushPopStaticLink.staticLinkDepth = symbol->distanceFromRoot;
                push->val.pushPopStaticLink.temporary = currentTemporary;
                appendInstructions(push);
                currentTemporary++;
            }

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
            appendInstructions(pop);
            currentTemporary++;

            if (symbol->distanceFromRoot == symbolTable->distanceFromRoot + 1) {
                //We need to restore the static link
                Instructions *popLink = newInstruction();
                popLink->kind = COMPLEX_RESTORE_STATIC_LINK;
                popLink->val.pushPopStaticLink.staticLinkDepth = symbol->distanceFromRoot;
                popLink->val.pushPopStaticLink.temporary = currentTemporary;
                appendInstructions(popLink);
                currentTemporary++;
            }
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
            //TODO DO this manually or MMX SSSE3 PABSD OR USE PROVIDED METHOD
            // mask = n >> (sizeof(int) * bitsof(char) - 1)
            // (mask + n)^mask
            size_t tempToAbsOn = generateInstructionsForExpression(term->val.absD.expression, symbolTable);

            //We must evaluate (sizeof(int) * bitsof(char) - 1)
            int bitsofChar = 8;
            int sizeInt = INTEGER_SIZE;
            int maskSize = bitsofChar * sizeInt - 1;

            //Make copy of n
            Instructions *copyN = newInstruction();
            copyN->kind = INSTRUCTION_COPY;
            copyN->val.arithmetic2.source = tempToAbsOn; //maskConstant
            copyN->val.arithmetic2.dest = currentTemporary;
            appendInstructions(copyN);
            size_t maskTemp = currentTemporary;
            currentTemporary++;

            // Bitmask right arithmetic shift
            Instructions *mask = newInstruction();
            mask->kind = INSTRUCTION_RIGHT_SHIFT;
            mask->val.rightShift.constant = maskSize; //maskConstant
            mask->val.rightShift.temp = maskTemp; //Ntemp is now the mask
            appendInstructions(mask);

            //Addition (mask + n) we can corrupt n
            Instructions *add = newInstruction();
            add->kind = INSTRUCTION_ADD;
            add->val.arithmetic2.source = maskTemp; //add mask
            add->val.arithmetic2.dest = tempToAbsOn;
            appendInstructions(add);

            //mask + n now resides in tempToAbsOn and mask in current - 1
            Instructions *xor = newInstruction();
            xor->kind = INSTRUCTION_XOR;
            xor->val.arithmetic2.source = tempToAbsOn;
            xor->val.arithmetic2.dest = maskTemp;
            appendInstructions(xor);

            return currentTemporary - 1;
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
            Instructions *num = newInstruction();
            num->kind = INSTRUCTION_CONST;
            num->val.constant.value = 0;
            num->val.constant.temp = currentTemporary;
            appendInstructions(num);
            currentTemporary++;
            return currentTemporary - 1;
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
    size_t toReturn = 0;

    Instructions *expStart = newInstruction();
    expStart->kind = METADATA_BEGIN_ARITHMETIC_EVALUATION;
    appendInstructions(expStart);

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
                    toReturn = rhsTemp;
                } break;
                case opDivK: {
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_DIV;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);
                    toReturn = rhsTemp;
                } break;
                case opPlusK: {
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_ADD;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);
                    toReturn = rhsTemp;
                } break;
                case opMinusK: {
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic2.source = rhsTemp;
                    instruction->val.arithmetic2.dest = lhsTemp;
                    appendInstructions(instruction);
                    toReturn = lhsTemp;
                } break;
                case opInequalityK: {
                    //subtract
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);
                    toReturn = rhsTemp;
                } break;
                case opEqualityK: {
                    //subtract equal check
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
                    trick->val.arithmetic2.source = rhsTemp; // For the 1 constant
                    trick->val.arithmetic2.dest = currentTemporary - 1; // For the resulting subtraction
                    appendInstructions(trick);

                    toReturn = currentTemporary - 1;
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
                    toReturn = rhsTemp;
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

                    toReturn = lhsTemp;
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

                    toReturn = rhsTemp;
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

                    toReturn = lhsTemp;
                } break;
                case opAndK: {
                    //and lhs and rhs
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_AND;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);
                    toReturn = rhsTemp;
                } break;
                case opOrK: {
                    //and lhs and rhs
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_OR;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);
                    toReturn = rhsTemp;
                } break;
            }
        }
            break;
        case termK: {
            toReturn = generateInstructionsForTerm(expression->val.termD.term, symbolTable);
        } break;
    }

    Instructions *expEnd = newInstruction();
    expEnd->kind = METADATA_END_ARITHMETIC_EVALUATION;
    appendInstructions(expEnd);

    return toReturn;
}

Instructions *newInstruction() {
    Instructions *ret = NEW(Instructions);

    ret->next = NULL;
    //ret->context = NULL;

    return ret;
}

//Assume var cannot be id
Type *applyVariableToType(Variable *variable, Type *toApplyTo) {
    switch (variable->kind) {
        case arrayIndexK: {
            return toApplyTo->val.arrayType.type;
        } break;
        case recordLookupK: {
            VarDelList *iter = toApplyTo->val.recordType.types;

            while (iter != NULL) {

                if (strcmp(iter->identifier, variable->val.recordLookupD.id) == 0) {
                    return iter->type;
                }

                iter = iter->next;
            }

        } break;
    }
}

Variable *dropNVariable(Variable *variable, int n) {
    Variable *iter = variable;

    while (n != 0) {
        switch (iter->kind) {
            case arrayIndexK: {
                iter = iter->val.arrayIndexD.var;
            } break;
            case recordLookupK: {
                iter = iter->val.recordLookupD.var;
            } break;
        }
        n--;
    }

    return iter;
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
        } break;
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
            size_t constNum = currentTemporary;
            currentTemporary++;

            SYMBOL *symbol = getSymbolForBaseVariable(statement->val.allocateD.var, statement->symbolTable);

            //size_t accessTemp = generateInstructionsForVariableAccess(statement->val.allocateD.var, statement->symbolTable);
            Type *type = unwrapVariable(statement->val.allocateD.var, statement->symbolTable);
            Instructions *ret = newInstruction();
            ret->kind = COMPLEX_ALLOCATE;
            ret->val.allocate.timesTemp = constNum;
            ret->val.allocate.ptrTemp = currentTemporary;
            ret->val.allocate.tpe = type;
            size_t allocPtrTemp = currentTemporary;
            appendInstructions(ret);
            currentTemporary++;

            //Instructions for getting getting the address we need to move the pointer to
            generateInstructionsForVariableSave(statement->val.allocateD.var, statement->symbolTable, allocPtrTemp);

            Instructions *endAlloc = newInstruction();
            endAlloc->kind = COMPLEX_ALLOCATE_END;
            appendInstructions(endAlloc);
        } break;
        case statAllocateLenK: {
            size_t lenExp = generateInstructionsForExpression(statement->val.allocateLenD.len, statement->symbolTable);
            SYMBOL *symbol = getSymbolForBaseVariable(statement->val.allocateD.var, statement->symbolTable);

            //size_t accessTemp = generateInstructionsForVariableAccess(statement->val.allocateD.var, statement->symbolTable);
            Type *type = unwrapVariable(statement->val.allocateD.var, statement->symbolTable);
            Instructions *ret = newInstruction();
            ret->kind = COMPLEX_ALLOCATE;
            ret->val.allocate.timesTemp = lenExp;
            ret->val.allocate.ptrTemp = currentTemporary;
            ret->val.allocate.tpe = type;
            size_t allocPtrTemp = currentTemporary;
            appendInstructions(ret);
            currentTemporary++;

            //Instructions for getting getting the address we need to move the pointer to
            generateInstructionsForVariableSave(statement->val.allocateD.var, statement->symbolTable, allocPtrTemp);

            Instructions *endAlloc = newInstruction();
            endAlloc->kind = COMPLEX_ALLOCATE_END;
            appendInstructions(endAlloc);
        } break;
        case statIfK: {
            //TODO
            Instructions *constant = newInstruction();
            constant->kind = INSTRUCTION_CONST;
            constant->val.constant.temp = currentTemporary;
            constant->val.constant.value = 1;
            appendInstructions(constant);
            size_t constantTemp = currentTemporary;
            currentTemporary++;

            size_t boolTemp = generateInstructionsForExpression(statement->val.ifD.exp, statement->symbolTable);
            Instructions *boolCmp = newInstruction();
            boolCmp->kind = INSTRUCTION_CMP;
            boolCmp->val.arithmetic2.source = boolTemp;
            boolCmp->val.arithmetic2.dest = constantTemp;
            appendInstructions(boolCmp);

            char *beginBuf = (char*)malloc(sizeof(char) * 32);
            sprintf(beginBuf, "if_%zu_begin", ifCounter);
            char *endBuf = (char*)malloc(sizeof(char) * 32);
            sprintf(endBuf, "if_%zu_end", ifCounter);
            ifCounter++;

            Instructions *je = newInstruction();
            je->kind = INSTRUCTION_JE;
            je->val.label = beginBuf;
            appendInstructions(je);

            Instructions *jmp = newInstruction();
            jmp->kind = INSTRUCTION_JMP;
            jmp->val.label = endBuf;
            appendInstructions(jmp);

            Instructions *begin = newInstruction();
            begin->kind = INSTRUCTION_LABEL;
            begin->val.label = beginBuf;
            appendInstructions(begin);

            generateInstructionTreeForStatement(statement->val.ifD.statement);

            Instructions *end = newInstruction();
            end->kind = INSTRUCTION_LABEL;
            end->val.label = endBuf;
            appendInstructions(end);

        } break;
        case statIfElK: {
            //TODO
            Instructions *constant = newInstruction();
            constant->kind = INSTRUCTION_CONST;
            constant->val.constant.temp = currentTemporary;
            constant->val.constant.value = 1;
            appendInstructions(constant);
            size_t constantTemp = currentTemporary;
            currentTemporary++;

            size_t boolTemp = generateInstructionsForExpression(statement->val.ifElD.exp, statement->symbolTable);
            Instructions *boolCmp = newInstruction();
            boolCmp->kind = INSTRUCTION_CMP;
            boolCmp->val.arithmetic2.source = boolTemp;
            boolCmp->val.arithmetic2.dest = constantTemp;
            appendInstructions(boolCmp);

            char *beginBuf = (char*)malloc(sizeof(char) * 32);
            sprintf(beginBuf, "if_%zu_begin", ifCounter);
            char *elBuf = (char*)malloc(sizeof(char) * 32);
            sprintf(elBuf, "el_%zu_begin", ifCounter);
            char *endBuf = (char*)malloc(sizeof(char) * 32);
            sprintf(endBuf, "ifel_%zu_end", ifCounter);
            ifCounter++;

            Instructions *je = newInstruction();
            je->kind = INSTRUCTION_JE;
            je->val.label = beginBuf;
            appendInstructions(je);

            Instructions *jmp = newInstruction();
            jmp->kind = INSTRUCTION_JMP;
            jmp->val.label = elBuf;
            appendInstructions(jmp);

            Instructions *begin = newInstruction();
            begin->kind = INSTRUCTION_LABEL;
            begin->val.label = beginBuf;
            appendInstructions(begin);

            generateInstructionTreeForStatement(statement->val.ifElD.statement);

            Instructions *gotoEnd = newInstruction();
            gotoEnd->kind = INSTRUCTION_JMP;
            gotoEnd->val.label = endBuf;
            appendInstructions(gotoEnd);

            Instructions *el = newInstruction();
            el->kind = INSTRUCTION_LABEL;
            el->val.label = elBuf;
            appendInstructions(el);

            generateInstructionTreeForStatement(statement->val.ifElD.elseStatement);

            Instructions *end = newInstruction();
            end->kind = INSTRUCTION_LABEL;
            end->val.label = endBuf;
            appendInstructions(end);
        } break;
        case statWhileK: {
            char *cndBuf = (char*)malloc(sizeof(char) * 32);
            sprintf(cndBuf, "while_cnd_%zu", whileCounter);
            char *beginBuf = (char*)malloc(sizeof(char) * 32);
            sprintf(beginBuf, "while_%zu_begin", whileCounter);
            char *endBuf = (char*)malloc(sizeof(char) * 32);
            sprintf(endBuf, "while_%zu_end", whileCounter);
            whileCounter++;

            Instructions *cndLbl = newInstruction();
            cndLbl->kind = INSTRUCTION_LABEL;
            cndLbl->val.label = cndBuf;
            appendInstructions(cndLbl);

            Instructions *constant = newInstruction();
            constant->kind = INSTRUCTION_CONST;
            constant->val.constant.temp = currentTemporary;
            constant->val.constant.value = 1;
            appendInstructions(constant);
            size_t constantTemp = currentTemporary;
            currentTemporary++;

            size_t boolTemp = generateInstructionsForExpression(statement->val.ifElD.exp, statement->symbolTable);
            Instructions *boolCmp = newInstruction();
            boolCmp->kind = INSTRUCTION_CMP;
            boolCmp->val.arithmetic2.source = boolTemp;
            boolCmp->val.arithmetic2.dest = constantTemp;
            appendInstructions(boolCmp);

            Instructions *je = newInstruction();
            je->kind = INSTRUCTION_JE;
            je->val.label = beginBuf;
            appendInstructions(je);

            Instructions *jmp = newInstruction();
            jmp->kind = INSTRUCTION_JMP;
            jmp->val.label = endBuf;
            appendInstructions(jmp);

            Instructions *begin = newInstruction();
            begin->kind = INSTRUCTION_LABEL;
            begin->val.label = beginBuf;
            appendInstructions(begin);

            generateInstructionTreeForStatement(statement->val.ifD.statement);

            Instructions *reset = newInstruction();
            reset->kind = INSTRUCTION_JMP;
            reset->val.label = cndBuf;
            appendInstructions(reset);

            Instructions *end = newInstruction();
            end->kind = INSTRUCTION_LABEL;
            end->val.label = endBuf;
            appendInstructions(end);
        } break;
        case stmListK: {
            StatementList *statementList = statement->val.stmListD.statementList;

            while (statementList != NULL) {
                generateInstructionTreeForStatement(statementList->statement);
                statementList = statementList->next;
            }
        } break;
        case assignmentK: {
            //For now we simply tell that temporary must be moved back to variable number x
            //Fetch variable
            SYMBOL *symbol = getSymbolForBaseVariable(statement->val.assignmentD.var, statement->symbolTable);

            size_t expressionTemp = generateInstructionsForExpression(statement->val.assignmentD.exp, statement->symbolTable);

            generateInstructionsForVariableSave(statement->val.assignmentD.var, statement->symbolTable, expressionTemp);
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
            SYMBOL *symbol = getSymbol(declaration->symbolTable, declaration->val.functionD.function->head->indentifier);

            Instructions *label = newInstruction();
            label->val.functionHead.label = declaration->val.functionD.function->head->indentifier;
            label->val.functionHead.distance = symbol->distanceFromRoot + 1;
            label->val.functionHead.temporary = currentTemporary;
            label->kind = INSTRUCTION_FUNCTION_LABEL;
            currentTemporary++;

            SymbolTable *st = NULL;
            if (declaration->val.functionD.function->body->declarationList != NULL) {
                st = declaration->val.functionD.function->body->declarationList->declaration->symbolTable;
            } else if (declaration->val.functionD.function->body->statementList != NULL) {
                st = declaration->val.functionD.function->body->statementList->statement->symbolTable;
            }
            label->val.functionHead.tableForFunction = st;

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
        }
            break;
        case declValK: {
            SYMBOL *symbol = getSymbol(declaration->symbolTable, declaration->val.valD.id);
            Instructions *ret = newInstruction();
            ret->kind = INSTRUCTION_VAR;
            ret->val.var = symbol;
            appendInstructions(ret);

            size_t expressionTemp = generateInstructionsForExpression(declaration->val.valD.rhs, declaration->symbolTable);

            Variable *tmpVar = NEW(Variable);
            tmpVar->kind = varIdK;
            tmpVar->val.idD.id = declaration->val.valD.id;

            generateInstructionsForVariableSave(tmpVar, declaration->symbolTable, expressionTemp);
        } break;
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

    //Bind the table for the body block


    if (instructionHead == NULL) {
        instructionHead = head;
        instructionHead->previous = NULL;
        currentInstruction = instructionHead;
    }


    while (declarationList != NULL && declarationList->declaration->kind != declValK) {
        generateInstructionTreeForDeclaration(declarationList->declaration);
        declarationList = declarationList->next;
    }

    if (createMain) {
        Instructions *declsEnd = newInstruction();
        declsEnd->kind = METADATA_CREATE_MAIN;
        SymbolTable *st = NULL;
        if (body->declarationList != NULL) {
            st = body->declarationList->declaration->symbolTable;
        } else if (body->statementList != NULL) {
            st = body->statementList->statement->symbolTable;
        }
        declsEnd->val.tableForFunction = st;

        appendInstructions(declsEnd);
    }

    declarationList = body->declarationList;
    while (declarationList != NULL) {
        if (declarationList->declaration->kind == declValK) {
            generateInstructionTreeForDeclaration(declarationList->declaration);
            declarationList = declarationList->next;
        }

        declarationList = declarationList->next;
    }

    while (statementList != NULL) {
        generateInstructionTreeForStatement(statementList->statement);
        statementList = statementList->next;
    }

    currentInstruction->next = newInstruction();
    currentInstruction = currentInstruction->next;
    currentInstruction->kind = METADATA_END_BODY_BLOCK;

    currentTemporary = restoreTemporary;

    return instructionHead;
}