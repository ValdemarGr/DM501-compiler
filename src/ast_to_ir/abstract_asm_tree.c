#include "abstract_asm_tree.h"
#include "../ast/tree.h"
#include "intermediate_representation.h"

static Instructions *instructionHead = NULL;
static Instructions *currentInstruction = NULL;

//If the context stack contains something we need to apply the instructions in the current context
static Stack *contextStack = NULL;
static size_t currentTemporary = 0;

Instructions *newInstruction() {
    Instructions *ret = NEW(Instructions);

    ret->next = NULL;
    ret->context = NULL;

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

//size_t is the resulting temporary
size_t generateInstructionsForExpression(Expression *expression) {
    switch (expression->kind) {
        case opK: {
            size_t lhsTemp = generateInstructionsForExpression(expression->val.op.left);
            size_t rhsTemp = generateInstructionsForExpression(expression->val.op.right);

            switch (expression->val.op.operator->kind) {
                case opMultK: {
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MUL;
                    instruction->val.arithmetic.lhsTemp = lhsTemp;
                    instruction->val.arithmetic.rhsTemp = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
                case opDivK: {
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_DIV;
                    instruction->val.arithmetic.lhsTemp = lhsTemp;
                    instruction->val.arithmetic.rhsTemp = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
                case opPlusK: {
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_ADD;
                    instruction->val.arithmetic.lhsTemp = lhsTemp;
                    instruction->val.arithmetic.rhsTemp = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
                case opMinusK: {
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic.lhsTemp = lhsTemp;
                    instruction->val.arithmetic.rhsTemp = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
                case opInequalityK: {
                    //subtract
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic.lhsTemp = lhsTemp;
                    instruction->val.arithmetic.rhsTemp = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
                case opEqualityK: {
                    //subtract and use 1-x trick to invert
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic.lhsTemp = lhsTemp;
                    instruction->val.arithmetic.rhsTemp = rhsTemp;
                    appendInstructions(instruction);

                    Instructions *constant = newInstruction();
                    constant->kind = INSTRUCTION_CONST;
                    constant->val.constant.value = 1;
                    constant->val.constant.temp = currentTemporary;
                    appendInstructions(constant);
                    currentTemporary++;

                    Instructions *trick = newInstruction();
                    trick->kind = INSTRUCTION_MINUS;
                    trick->val.arithmetic.lhsTemp = currentTemporary - 1; // For the 1 constant
                    trick->val.arithmetic.rhsTemp = rhsTemp; // For the resulting subtraction
                    appendInstructions(trick);

                    return rhsTemp;
                } break;
                case opGreaterK: {
                    //x > y -> lhs > rhs
                    //subtract x from y and check if the result is > 0 later
                    //also constrain to boolean
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic.lhsTemp = lhsTemp;
                    instruction->val.arithmetic.rhsTemp = rhsTemp;
                    appendInstructions(instruction);

                    Instructions *constrain = newInstruction();
                    constrain->kind = COMPLEX_CONSTRAIN_BOOLEAN;
                    constrain->val.tempToConstrain = rhsTemp;
                    appendInstructions(constrain);
                    return rhsTemp;
                } break;
                case opLessK: {
                    //Same as above, but lhs and rhs flipped
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic.lhsTemp = rhsTemp;
                    instruction->val.arithmetic.rhsTemp = lhsTemp;
                    appendInstructions(instruction);

                    Instructions *constrain = newInstruction();
                    constrain->kind = COMPLEX_CONSTRAIN_BOOLEAN;
                    constrain->val.tempToConstrain = lhsTemp;
                    appendInstructions(constrain);

                    return lhsTemp;
                } break;
                case opGeqK: {
                    //Can be hacked by using greater but adding a constant of one
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic.lhsTemp = lhsTemp;
                    instruction->val.arithmetic.rhsTemp = rhsTemp;
                    appendInstructions(instruction);

                    Instructions *constant = newInstruction();
                    constant->kind = INSTRUCTION_CONST;
                    constant->val.constant.value = 1;
                    constant->val.constant.temp = currentTemporary;
                    appendInstructions(constant);
                    currentTemporary++;

                    Instructions *trick = newInstruction();
                    trick->kind = INSTRUCTION_ADD;
                    trick->val.arithmetic.lhsTemp = currentTemporary - 1; // For the 1 constant
                    trick->val.arithmetic.rhsTemp = rhsTemp; // For the resulting subtraction
                    appendInstructions(trick);

                    Instructions *constrain = newInstruction();
                    constrain->kind = COMPLEX_CONSTRAIN_BOOLEAN;
                    constrain->val.tempToConstrain = rhsTemp;
                    appendInstructions(constrain);

                    return rhsTemp;
                } break;
                case opLeqK: {
                    //Same as above, but lhs and rhs flipped
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic.lhsTemp = rhsTemp;
                    instruction->val.arithmetic.rhsTemp = lhsTemp;
                    appendInstructions(instruction);

                    Instructions *constant = newInstruction();
                    constant->kind = INSTRUCTION_CONST;
                    constant->val.constant.value = 1;
                    constant->val.constant.temp = currentTemporary;
                    appendInstructions(constant);
                    currentTemporary++;

                    Instructions *trick = newInstruction();
                    trick->kind = INSTRUCTION_ADD;
                    trick->val.arithmetic.lhsTemp = currentTemporary - 1; // For the 1 constant
                    trick->val.arithmetic.rhsTemp = lhsTemp; // For the resulting subtraction
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
                    instruction->val.arithmetic.lhsTemp = lhsTemp;
                    instruction->val.arithmetic.rhsTemp = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
                case opOrK: {
                    //and lhs and rhs
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_OR;
                    instruction->val.arithmetic.lhsTemp = lhsTemp;
                    instruction->val.arithmetic.rhsTemp = rhsTemp;
                    appendInstructions(instruction);
                    return rhsTemp;
                } break;
            }
        }
            break;
        case termK:
            break;
    }
}

void generateInstructionTreeForStatement(Statement *statement) {


    switch (statement->kind) {
        case statReturnK: {
            Context *currentContext = NEW(Context);
            currentContext->kind = ACCUMULATE;
            currentContext->val.temp = currentTemporary;

            push(contextStack, currentContext);

            size_t resultingTemporary = generateInstructionsForExpression(statement->val.returnD.exp);

            Instructions *instructions = newInstruction();
            instructions->kind = INSTRUCTION_RETURN;
            instructions->val.tempToReturn = resultingTemporary;
            appendInstructions(instructions);

            pop(contextStack);

            currentTemporary++;
        }
            break;
        case statWriteK: {
            Instructions *instructions = newInstruction();
            instructions->kind = INSTRUCTION_WRITE;
            instructions->val.tempToWrite = generateInstructionsForExpression(statement->val.writeD.exp);
        }
            break;
        case statAllocateK:
            break;
        case statAllocateLenK:
            break;
        case statIfK:
            break;
        case statIfElK:
            break;
        case statWhileK:
            break;
        case stmListK:
            break;
        case assignmentK:
            break;
    }
}

void generateInstructionTreeForDeclaration(Declaration *declaration) {

    switch (declaration->kind) {
        case declVarK: {
            SYMBOL *symbol = getSymbol(declaration->symbolTable, declaration->val.varD.id);
            Instructions *ret = newInstruction();
            ret->kind = INSTRUCTION_VAR;
            ret->val.var = symbol->uniqueId;

            appendInstructions(ret);
        }
        break;
        case declVarsK: {
            //For current var
            Declaration *thisVar = declaration->val.varsD.var;

            SYMBOL *symbol = getSymbol(declaration->symbolTable, thisVar->val.varD.id);
            Instructions *ret = newInstruction();
            ret->kind = INSTRUCTION_VAR;
            ret->val.var = symbol->uniqueId;

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
            int counter = 0;

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
            ret->kind = INSTRUCTION_VAL;
            ret->val.val = symbol->uniqueId;

            appendInstructions(ret);
        }
            break;
        case declClassK:
            //TODO
            break;
    }
}

void generateInstructionTree(Body *body) {
    //Save temporary counter
    size_t restoreTemporary = currentTemporary;

    DeclarationList *declarationList = body->declarationList;
    StatementList *statementList = body->statementList;
    Instructions *head = newInstruction();
    head->kind = METADATA_BEGIN_BODY_BLOCK;

    if (instructionHead == NULL) {
        instructionHead = head;
        currentInstruction = instructionHead;
    }

    if (contextStack == NULL) {
        contextStack = initStack();
    }

    //Functions will take tail and return the new tail, head must be pre-inited

    while (declarationList != NULL) {
        generateInstructionTreeForDeclaration(declarationList->declaration);
        declarationList = declarationList->next;
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
}