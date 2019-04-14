#include "abstract_asm_tree.h"
#include "../ast/tree.h"

Instructions *generateInstructionTreeForStatement(Statement *statement, Instructions *last) {


    switch (statement->kind) {
        case statReturnK: {
            Instructions *instructions = NEW(Instructions);
            instructions->next = NULL;
            instructions->kind = INSTRUCTION_RETURN;

            last->next = instructions;
            return instructions;
        }
            break;
        case statWriteK:
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

    return NULL;
}

Instructions *generateInstructionTreeForDeclaration(Declaration *declaration, Instructions *last) {

    switch (declaration->kind) {
        case declVarK: {
            SYMBOL *symbol = getSymbol(declaration->symbolTable, declaration->val.varD.id);
            Instructions *ret = NEW(Instructions);
            ret->next = NULL;
            ret->kind = INSTRUCTION_VAR;
            ret->val.var = symbol->uniqueId;


            last->next = ret;

            return ret;
        }
        break;
        case declVarsK: {
            //For current var
            Declaration *thisVar = declaration->val.varsD.var;

            SYMBOL *symbol = getSymbol(declaration->symbolTable, thisVar->val.varD.id);
            Instructions *ret = NEW(Instructions);
            ret->next = NULL;
            ret->kind = INSTRUCTION_VAR;
            ret->val.var = symbol->uniqueId;

            last->next = ret;

            if (declaration->val.varsD.next == NULL) {
                return ret;
            } else {
                return generateInstructionTreeForDeclaration(declaration->val.varsD.next, ret);
            }
        }
        break;
        case declTypeK:
            //SKIP
            return last;
            break;
        case declFuncK: {
            //For function label
            Instructions *label = NEW(Instructions);
            label->next = NULL;
            label->val.label = declaration->val.functionD.function->head->indentifier;
            label->kind = INSTRUCTION_FUNCTION_LABEL;

            //Readjust arg
            last->next = label;

            //For args we generate metadata for later (maybe this is useless, who knows)
            DeclarationList *declarationList = declaration->val.functionD.function->head->declarationList;
            Instructions *current = label;
            int counter = 0;

            while (declarationList != NULL) {
                //Create arg instr for this argument
                Instructions *arg = NEW(Instructions);
                arg->next = NULL;
                arg->kind = METADATA_FUNCTION_ARGUMENT;
                arg->val.argNum = counter;
                current->next = arg;

                current = current->next;
                declarationList = declarationList->next;
                counter++;
            }

            current = generateInstructionTree(declaration->val.functionD.function->body, current);

            Instructions *tail = NEW(Instructions);
            tail->next = NULL;
            tail->kind = INSTRUCTION_FUNCTION_END;
            tail->val.label = declaration->val.functionD.function->tail->indentifier;
            current->next = tail;

            return tail;
        }
            break;
        case declValK:
            break;
        case declClassK:
            break;
    }

    return NULL;
}

Instructions *generateInstructionTree(Body *body, Instructions *preBodyLast) {
    DeclarationList *declarationList = body->declarationList;
    StatementList *statementList = body->statementList;
    Instructions *head = NEW(Instructions);
    head->next = NULL;
    head->kind = METADATA_BEGIN_BODY_BLOCK;
    preBodyLast->next = head;

    Instructions *last = head;

    //Functions will take tail and return the new tail, head must be pre-inited

    while (declarationList != NULL) {
        last = generateInstructionTreeForDeclaration(declarationList->declaration, last);

        declarationList = declarationList->next;
    }

    while (statementList != NULL) {
        last = generateInstructionTreeForStatement(statementList->statement, last);

        statementList = statementList->next;
    }

    last->next = NEW(Instructions);
    last = last->next;
    last->next = NULL;
    last->kind = METADATA_END_BODY_BLOCK;

    return last;
}