#include "abstract_asm_tree.h"



static Instructions *instructionHead = NULL;
static Instructions *currentInstruction = NULL;
static bool mainCreated = false;
static size_t ifCounter = 0;

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

            if (frameStackDistanceToVariable == 0) {
                Instructions *load = newInstruction();
                load->kind = COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK;
                load->val.ptrLoad.var = symbol;
                load->val.ptrLoad.temporary = currentTemporary;
                currentTemporary++;
                appendInstructions(load);
                return currentTemporary - 1;
            } else {
                //TODO Maybe use accumulator for counting "back in memory" offset
                Instructions *fetch = newInstruction();
                fetch->kind = COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE;
                fetch->val.loadTempFromParentScope.uniqueVariableId = uniqueVariableId;
                fetch->val.loadTempFromParentScope.scopeToFindFrame = symbol->distanceFromRoot;
                fetch->val.loadTempFromParentScope.outputTemp = currentTemporary;
                appendInstructions(fetch);
                currentTemporary++;
                return currentTemporary - 1;
            }
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

                    toReturn = rhsTemp;
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
            appendInstructions(ret);
            currentTemporary++;

            Instructions *loadPtr = newInstruction();
            loadPtr->kind = COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME;
            loadPtr->val.loadPtrToStaticLink.ptrTemp = currentTemporary - 1;
            loadPtr->val.loadPtrToStaticLink.linkBaseOffset = symbol->uniqueIdForScope;
            loadPtr->val.loadPtrToStaticLink.scopeToFindFrame = symbol->distanceFromRoot;
            loadPtr->val.loadPtrToStaticLink.intermediateTemp = currentTemporary;
            appendInstructions(loadPtr);
            currentTemporary++;

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
            appendInstructions(ret);
            currentTemporary++;

            Instructions *loadPtr = newInstruction();
            loadPtr->kind = COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME;
            loadPtr->val.loadPtrToStaticLink.ptrTemp = currentTemporary - 1;
            loadPtr->val.loadPtrToStaticLink.linkBaseOffset = symbol->uniqueIdForScope;
            loadPtr->val.loadPtrToStaticLink.scopeToFindFrame = symbol->distanceFromRoot;
            loadPtr->val.loadPtrToStaticLink.intermediateTemp = currentTemporary;
            appendInstructions(loadPtr);
            currentTemporary++;

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

            //cmp
            //je label
            //jmp end
            //label
            //instrForIf
            //end

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
        case statIfElK:
            //TODO
            break;
        case statWhileK:
            //TODO
            break;
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
            //Todo unwrap correctly
            SYMBOL *symbol = getSymbolForBaseVariable(statement->val.assignmentD.var, statement->symbolTable);

            size_t expressionTemp = generateInstructionsForExpression(statement->val.assignmentD.exp, statement->symbolTable);

            size_t frameStackDistanceToVariable = statement->symbolTable->distanceFromRoot - symbol->distanceFromRoot;

            if (frameStackDistanceToVariable == 0) {
                Instructions *save = newInstruction();
                save->kind = COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER;
                save->val.ptrSave.sym = symbol;
                save->val.ptrSave.tempValue = expressionTemp;
                save->val.ptrSave.intermediate = currentTemporary;
                appendInstructions(save);
                currentTemporary++;
            } else {
                Instructions *save = newInstruction();
                save->kind = COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE;
                save->val.saveTempFromParentScope.uniqueVariableId = symbol->uniqueIdForScope;
                save->val.saveTempFromParentScope.scopeToFindFrame = symbol->distanceFromRoot;
                save->val.saveTempFromParentScope.inputTemp = expressionTemp;
                save->val.saveTempFromParentScope.intermediateTemp = currentTemporary;
                appendInstructions(save);
                currentTemporary++;
            }
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
        instructionHead->previous = NULL;
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