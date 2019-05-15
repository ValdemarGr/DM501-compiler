//
// Created by valde on 4/23/19.
//

#include "peephole.h"
#include "../ast_to_ir/intermediate_representation.h"

Instructions *skipToNextImportantInstruction(Instructions *instructions);

void addInstructionTemplate(PeepholeTemplates *peepholeTemplates, SimpleInstruction *simpleInstruction, PeepholeApplyType apply, int size) {
    PeepholeTemplates *last = peepholeTemplates->last;

    PeepholeTemplates *newTemplates;
    if (last == NULL) {
        newTemplates = peepholeTemplates;
        peepholeTemplates->next = newTemplates;
    } else {
        newTemplates = NEW(PeepholeTemplates);
        peepholeTemplates->last->next = newTemplates;
    }

    newTemplates->next = NULL;
    newTemplates->simpleInstruction = simpleInstruction;
    newTemplates->apply = apply;
    newTemplates->size = size;

    peepholeTemplates->last = newTemplates;
}

void appendInstruction(SimpleInstruction *simpleInstruction, SimpleInstruction *new) {
    new->next = NULL;
    simpleInstruction->next = new;
}

void appendTemplates(PeepholeTemplates *peepholeTemplates, PeepholeTemplates *new) {
    new->next = NULL;
    peepholeTemplates->next = new;
}

PeepholeTemplates *generateRulesetsForSize() {
    PeepholeTemplates *peepholeTemplates = NEW(PeepholeTemplates);
    peepholeTemplates->last = NULL;

    {
        size_t registerTrackerForBlock = ANY + 1;

        SimpleInstruction *movConst = NEW(SimpleInstruction);
        movConst->kind = INSTRUCTION_CONST;
        registerTrackerForBlock++;

        SimpleInstruction *simpleAdd = NEW(SimpleInstruction);
        simpleAdd->kind = INSTRUCTION_ADD;
        registerTrackerForBlock++;
        appendInstruction(movConst, simpleAdd);
        addInstructionTemplate(peepholeTemplates, movConst, REMOVE_CONST_REGISTER_ADD, 2);
    }

    {
        size_t registerTrackerForBlock = ANY + 1;

        SimpleInstruction *movConst = NEW(SimpleInstruction);
        movConst->kind = INSTRUCTION_CONST;
        registerTrackerForBlock++;

        SimpleInstruction *simpleMul = NEW(SimpleInstruction);
        simpleMul->kind = INSTRUCTION_MUL;
        registerTrackerForBlock++;
        appendInstruction(movConst, simpleMul);
        addInstructionTemplate(peepholeTemplates, movConst, REMOVE_CONST_REGISTER_MUL, 2);
    }

    {
        size_t registerTrackerForBlock = ANY + 1;

        SimpleInstruction *push = NEW(SimpleInstruction);
        push->kind = INSTRUCTION_PUSH;
        registerTrackerForBlock++;

        SimpleInstruction *pop = NEW(SimpleInstruction);
        pop->kind = INSTRUCTION_POP;
        registerTrackerForBlock++;
        appendInstruction(push, pop);
        addInstructionTemplate(peepholeTemplates, push, REMOVE_PUSH_POP, 2);
    }

    {
        size_t registerTrackerForBlock = ANY + 1;

        SimpleInstruction *move = NEW(SimpleInstruction);
        move->kind = INSTRUCTION_CONST;
        registerTrackerForBlock++;

        addInstructionTemplate(peepholeTemplates, move, CONST_ZERO_TO_SET_ZERO, 1);
    }

    return peepholeTemplates;
}

Instructions *localAppendInstructions(Instructions *new) {
    Instructions *iter = new;

    while (iter->next != NULL) {
        iter = iter->next;
    }

    return iter;
}
/*
size_t smallestRegister(PeepholeTemplates *template, Instructions *instructions) {
    Instructions *iter = instructions;
    SimpleInstruction *simpleCounter = template->simpleInstruction;

    int smallestValue = INT_MAX;

    switch (template->apply) {
        case REMOVE_CONST_REGISTER_ADD: {
            smallestValue = MIN((int)iter->val.constant.temp, smallestValue);
            iter = iter->next;
            smallestValue = MIN((int)iter->val.arithmetic2.source, smallestValue);
            smallestValue = MIN((int)iter->val.arithmetic2.dest, smallestValue);
        } break;
        case REMOVE_CONST_REGISTER_MUL: {
            smallestValue = MIN((int)iter->val.constant.temp, smallestValue);
            iter = iter->next;
            smallestValue = MIN((int)iter->val.arithmetic2.source, smallestValue);
            smallestValue = MIN((int)iter->val.arithmetic2.dest, smallestValue);
        } break;
    }

    return (size_t)smallestValue;
}
*/

bool templateMatch(Instructions *head, PeepholeTemplate *template) {
    Kinds *kind = template->kinds;
    Instructions *instruction = head;

    while(kind != NULL && instruction != NULL) {
        if (instruction->kind != kind->kind) {
            return false;
        }
        kind = kind->next;
        instruction = skipToNextImportantInstruction(instruction);
    }

    /*
     * The template requires more instructions than there is left
     */
    if (kind != NULL) {
        return false;
    }

    return true;
}

//Replacement patterns
Instructions *applyTemplate(SimpleInstruction *simpleHead, Instructions *instrHead, size_t n, PeepholeApplyType apply) {//, size_t min) {
    SimpleInstruction *simpleIter = simpleHead;
    Instructions *instructionsIter = instrHead;

    Instructions *instructionHead = NULL;
    Instructions *currentInstruction = NULL;
    bool templateApplied = false;

    switch (apply) {
        case REMOVE_CONST_REGISTER_ADD: {
            if (instructionsIter->val.constant.temp == skipToNextImportantInstruction(instructionsIter->next)->val.arithmetic2.source) {
                instructionHead = newInstruction();
                instructionHead->kind = INSTRUCTION_ADD_CONST;
                instructionHead->val.art2const.constant = instructionsIter->val.constant.value;
                instructionsIter =  skipToNextImportantInstruction(instructionsIter->next);
                instructionHead->val.art2const.temp = instructionsIter->val.arithmetic2.dest;
                currentInstruction = instructionHead;
                templateApplied = true;
            }
        } break;
        case REMOVE_CONST_REGISTER_MUL: {
            if (instructionsIter->val.constant.temp == skipToNextImportantInstruction(instructionsIter->next)->val.arithmetic2.source) {
                instructionHead = newInstruction();
                instructionHead->kind = INSTRUCTION_MUL_CONST;
                instructionHead->val.art2const.constant = instructionsIter->val.constant.value;
                instructionsIter = skipToNextImportantInstruction(instructionsIter->next);
                instructionHead->val.art2const.temp = instructionsIter->val.arithmetic2.dest;
                currentInstruction = instructionHead;
                templateApplied = true;
            }
        } break;
        case REMOVE_PUSH_POP: {
            if (instructionsIter->val.tempToPush == instructionsIter->val.tempToPopInto) {
                instructionHead = NULL;
                currentInstruction = instructionHead;
                templateApplied = true;
            }
        } break;
        case CONST_ZERO_TO_SET_ZERO: {
            if (instructionsIter->val.constant.value == 0) {
                instructionHead = newInstruction();
                instructionHead->kind = INSTRUCTION_SET_ZERO;
                instructionHead->val.tempToSetZero = instructionsIter->val.constant.temp;
                currentInstruction = instructionHead;
                templateApplied = true;
            }
        } break;
        default: break;
    }

    if (templateApplied) {
        //Replace entire context
        Instructions *toReplaceFrom = instrHead->previous;
        Instructions *toReplaceTo = instrHead;

        for (int i = 0; i < n; ++i) {
            toReplaceTo = skipToNextImportantInstruction(toReplaceTo->next);
        }

        if (instructionHead == NULL) {
            toReplaceTo->previous = toReplaceFrom;
            toReplaceFrom->next = toReplaceTo;
        } else {
            toReplaceFrom->next = instructionHead;
            currentInstruction->next = toReplaceTo;
            currentInstruction->previous = instructionHead;
            toReplaceTo->previous = currentInstruction;
        }

        return toReplaceFrom;
    }
    return instrHead;
}

Instructions *skipToNextImportantInstruction(Instructions *instructions) {
    switch (instructions->kind) {
        case INSTRUCTION_ADD: { return instructions; } break;
        case INSTRUCTION_MINUS: { return instructions; } break;
        case INSTRUCTION_MUL: {  return instructions; } break;
        case INSTRUCTION_DIV: { return instructions;  } break;
        case INSTRUCTION_CONST: {  return instructions; } break;
        case INSTRUCTION_PROGRAM_BEGIN: { return instructions;  } break;
        case INSTRUCTION_FUNCTION_LABEL: {  return instructions; } break;
        case INSTRUCTION_VAR: {  return instructions; } break;
        case INSTRUCTION_FUNCTION_END: { return instructions;  } break;
        case INSTRUCTION_RETURN: {  return instructions; } break;
        case INSTRUCTION_WRITE: { return instructions;  } break;
        case INSTRUCTION_AND: {  return instructions; } break;
        case INSTRUCTION_OR: {  return instructions; } break;
        case INSTRUCTION_PUSH: {  return instructions; } break;
        case INSTRUCTION_POP: {  return instructions; } break;
        case INSTRUCTION_NEGATE: {  return instructions; } break;
        case INSTRUCTION_ABS: { return instructions;  } break;
        case INSTRUCTION_FUNCTION_CALL: { return instructions;} break;
        case INSTRUCTION_RIGHT_SHIFT: { return instructions;} break;
        case INSTRUCTION_XOR: { return instructions;} break;
        case INSTRUCTION_COPY: { return instructions;} break;
        case INSTRUCTION_CMP: {return instructions; } break;
        case INSTRUCTION_LABEL: { return instructions;} break;
        case INSTRUCTION_JE: { return instructions;} break;
        case INSTRUCTION_JMP: { return instructions;} break;
        case INSTRUCTION_MOVE: {return instructions; } break;
        case INSTRUCTION_ADD_CONST: { return instructions;} break;
        case INSTRUCTION_MUL_CONST: { return instructions;} break;
        case COMPLEX_ALLOCATE: {return instructions; } break;
        case COMPLEX_ALLOCATE_END: { return instructions;} break;
        case COMPLEX_CONSTRAIN_BOOLEAN: { return instructions;} break;
        case COMPLEX_SAVE_STATIC_LINK: { return instructions;} break;
        case COMPLEX_RESTORE_STATIC_LINK: { return instructions;} break;
        case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME: { return instructions;} break;
        case METADATA_BEGIN_BODY_BLOCK: { return instructions; } break; //<-- this is holy
        case METADATA_END_BODY_BLOCK: { return instructions;} break;
        case METADATA_FUNCTION_ARGUMENT: { return instructions;} break;
        case METADATA_CREATE_MAIN: { return instructions;} break;
        case METADATA_BEGIN_ARITHMETIC_EVALUATION: { return skipToNextImportantInstruction(instructions->next);} break;
        case METADATA_END_ARITHMETIC_EVALUATION: {return skipToNextImportantInstruction(instructions->next); } break;
        case INSTRUCTION_PUSH_STACK: { return instructions; } break;
        case INSTRUCTION_POP_STACK: { return instructions; } break;
        case INSTRUCTION_MOVE_TO_OFFSET: { return instructions; } break;
        case INSTRUCTION_LEA_TO_OFFSET: { return instructions; } break;
        case INSTRUCTION_REGISTER_CALL: { return instructions; } break;
        case COMPLEX_MOVE_TEMPORARY_INTO_STACK: { return instructions; } break;
        case COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE: { return instructions; } break;
        case COMPLEX_MOVE_TEMPORARY_FROM_STACK: { return instructions; } break;
        case COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE: { return instructions; } break;
        case COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET: { return instructions; } break;
        case COMPLEX_RIP_LAMBDA_LOAD: { return instructions; } break;
        case METADATA_BEGIN_GLOBAL_BLOCK: { return instructions; } break;
        case METADATA_END_GLOBAL_BLOCK: { return instructions; } break;
        case METADATA_DEBUG_INFO: { return skipToNextImportantInstruction(instructions->next); } break;
        case COMPLEX_GARBAGE_COLLECT: { return instructions; } break;
        case INSTRUCTION_ADD_STACK_PTR: { return instructions; } break;
        case INSTRUCTION_SET_ZERO: { return instructions; } break;
        case COMPLEX_ABS_VALUE: { return instructions; } break;
        case COMPLEX_SAVE_ALL: { return instructions; } break;
        case COMPLEX_RESTORE_ALL: { return instructions; } break;
        case INSTRUCTION_WRITE_CHAR: { return instructions; } break;
        case INSTRUCTION_WRITE_NL: { return instructions; } break;
    }
}

void peephole(Instructions *instructions) {
    PeepholeTemplates *peepholeTemplates = generateRulesetsForSize();

    Instructions *iter = instructions;
    bool notEqual;

    while (iter != NULL) {
        PeepholeTemplates *templateIter = peepholeTemplates;

        while (templateIter != NULL) {
            SimpleInstruction *simpleInstructionIter = templateIter->simpleInstruction;
            Instructions *realInstructionIter = iter;
            notEqual = false;

            //Test equality of templates, this includes register layout
            while (simpleInstructionIter != NULL && realInstructionIter != NULL) {
                if (simpleInstructionIter->kind != realInstructionIter->kind) {
                    notEqual = true;
                    break;
                }
                realInstructionIter = skipToNextImportantInstruction(realInstructionIter->next);
                simpleInstructionIter = simpleInstructionIter->next;
            }

            //Null test
            if (simpleInstructionIter != NULL || notEqual) {
                templateIter = templateIter->next;
                continue;
            }

            /*
            simpleInstructionIter = templateIter->simpleInstruction;
            realInstructionIter = iter;
            size_t smallestReg = smallestRegister(templateIter, realInstructionIter);*/

            //Apply template
            simpleInstructionIter = templateIter->simpleInstruction;
            realInstructionIter = iter;

            iter = applyTemplate(simpleInstructionIter, realInstructionIter, (size_t)templateIter->size, templateIter->apply);//, smallestReg);

            templateIter = templateIter->next;
        }

        iter = iter->next;
    }
}
