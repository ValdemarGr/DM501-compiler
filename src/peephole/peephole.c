//
// Created by valde on 4/23/19.
//

#include "peephole.h"
#include "../ast_to_ir/intermediate_representation.h"
#include "../register_allocation/liveness_analysis.h"

Instructions *skipToNextImportantInstruction(Instructions *instructions);

bool instructionsChanged;
Instructions *lookForDupFetches(Instructions *original, Instructions *current, InstructionKind ik) {
    if (current->kind != ik) {
        return lookForDupFetches(original, current->previous, ik);
    }

    switch (ik) {
        case INSTRUCTION_RETURN:
        case INSTRUCTION_FUNCTION_END:
        case INSTRUCTION_FUNCTION_CALL:
        case INSTRUCTION_LABEL:
        case INSTRUCTION_JE:
        case INSTRUCTION_JMP:
        case INSTRUCTION_REGISTER_CALL:
        case COMPLEX_RIP_LAMBDA_LOAD:
        case METADATA_BEGIN_BODY_BLOCK:
        case METADATA_END_BODY_BLOCK:
        case COMPLEX_RESTORE_ALL:
        case METADATA_CREATE_MAIN:
        case COMPLEX_SAVE_ALL:
        case INSTRUCTION_FUNCTION_LABEL: return NULL;
        case COMPLEX_MOVE_TEMPORARY_FROM_STACK: {
            if (
                    current->val.tempFromStack.offset == original->val.tempFromStack.offset &&
                    current->val.tempFromStack.inputTemp == original->val.tempFromStack.inputTemp
                    ) {
                return current;
            }
        } break;
        case COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE: {
            if (
                    current->val.tempFromStackScope.offset == original->val.tempFromStackScope.offset &&
                    current->val.tempFromStackScope.inputTemp == original->val.tempFromStackScope.inputTemp &&
                    current->val.tempFromStackScope.scopeToFindFrame == original->val.tempFromStackScope.scopeToFindFrame
                    ) {
                return current;
            }
        } break;
        case COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET: {
            if (
                    current->val.dereferenceOffset.offsetTemp == original->val.dereferenceOffset.offsetTemp &&
                    current->val.dereferenceOffset.ptrTemp == original->val.dereferenceOffset.ptrTemp
                    ) {
                return current;
            }
        } break;
    }

    return lookForDupFetches(original, current->previous, ik);
}

Instructions *fetchPreviousInstructionThatModifiesRegister(Instructions *current, size_t regToLookFor) {
    switch (current->kind) {
        case INSTRUCTION_RETURN:
        case INSTRUCTION_FUNCTION_END:
        case INSTRUCTION_FUNCTION_CALL:
        case INSTRUCTION_LABEL:
        case INSTRUCTION_JE:
        case INSTRUCTION_JMP:
        case INSTRUCTION_REGISTER_CALL:
        case COMPLEX_RIP_LAMBDA_LOAD:
        case METADATA_BEGIN_BODY_BLOCK:
        case METADATA_END_BODY_BLOCK:
        case COMPLEX_RESTORE_ALL:
        case METADATA_CREATE_MAIN:
        case COMPLEX_SAVE_ALL:
        case INSTRUCTION_FUNCTION_LABEL: return NULL;
        case INSTRUCTION_PROGRAM_BEGIN:
        case INSTRUCTION_VAR:
        case INSTRUCTION_WRITE:
        case INSTRUCTION_WRITE_CHAR:
        case INSTRUCTION_WRITE_NL:
        case INSTRUCTION_PUSH:
        case INSTRUCTION_PUSH_STACK:
        case INSTRUCTION_LEA_TO_OFFSET:
        case INSTRUCTION_ADD_STACK_PTR:
        case COMPLEX_ALLOCATE_END:
        case METADATA_END_ARITHMETIC_EVALUATION:
        case METADATA_BEGIN_ARITHMETIC_EVALUATION:
        case METADATA_BEGIN_GLOBAL_BLOCK:
        case METADATA_END_GLOBAL_BLOCK:
        case COMPLEX_MOVE_TEMPORARY_INTO_STACK:
        case RUNTIME_ARRAY_BOUNDS_CHECK:
        case RUNTIME_NEGATIVE_ALLOC:
        case RUNTIME_NULLPTR_CHECK:
        case RUNTIME_DIV_ZERO:
        case METADATA_DEBUG_INFO:
        case COMPLEX_GARBAGE_COLLECT:
        case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME:
        case INSTRUCTION_POP_STACK:break;
        case INSTRUCTION_DIV:
        case INSTRUCTION_MINUS:
        case INSTRUCTION_MUL:
        case INSTRUCTION_AND:
        case INSTRUCTION_OR:
        case INSTRUCTION_XOR:
        case INSTRUCTION_COPY:
        case INSTRUCTION_CMP:
        case INSTRUCTION_MOVE:
        case INSTRUCTION_LEA_ADD:
        case INSTRUCTION_ADD: if (current->val.arithmetic2.dest == regToLookFor) return current; break;
        case INSTRUCTION_CONST: if (current->val.constant.temp == regToLookFor) return current; break;
        case INSTRUCTION_POP: if (current->val.tempToPopInto == regToLookFor) return current; break;
        case INSTRUCTION_NEGATE: if (current->val.tempToNegate == regToLookFor) return current; break;
        case INSTRUCTION_RIGHT_SHIFT: if (current->val.rightShift.temp == regToLookFor) return current; break;
        case INSTRUCTION_MUL_CONST:
        case INSTRUCTION_LEA_ADD_CONST:
        case INSTRUCTION_ADD_CONST: if (current->val.art2const.temp == regToLookFor) return current; break;
        case INSTRUCTION_MOVE_TO_OFFSET: if (current->val.moveToOffset.ptrTemp == regToLookFor) return current; break;
        case INSTRUCTION_SET_ZERO: if (current->val.tempToSetZero == regToLookFor) return current; break;
        case COMPLEX_ALLOCATE: if (current->val.allocate.intermediate == regToLookFor) return NULL; break;
        case COMPLEX_CONSTRAIN_BOOLEAN: if (current->val.tempToConstrain == regToLookFor) return current; break;
        case COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE:{
            if (current->val.tempIntoStackScope.intermediate == regToLookFor ||
                    current->val.tempIntoStackScope.intermediate2 == regToLookFor) {
                return NULL;
            }
        } break;
        case COMPLEX_MOVE_TEMPORARY_FROM_STACK: if (current->val.tempFromStack.inputTemp == regToLookFor) return current; break;
        case COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE: {
            if (current->val.tempFromStackScope.intermediate == regToLookFor ||
                current->val.tempFromStackScope.intermediate2 == regToLookFor) {
                return NULL;
            } else if (current->val.tempFromStackScope.inputTemp == regToLookFor) {
                return current;
            }
        } break;
        case COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET: if (current->val.dereferenceOffset.ptrTemp == regToLookFor) return current; break;
        case COMPLEX_SAVE_STATIC_LINK: if (current->val.pushPopStaticLink.temporary == regToLookFor) return current; break;
        case COMPLEX_RESTORE_STATIC_LINK: if (current->val.pushPopStaticLink.temporary == regToLookFor) return current; break;
        case COMPLEX_ABS_VALUE: {
            if (current->val.arithmetic2.dest == regToLookFor ||
            current->val.arithmetic2.source == regToLookFor) {
                return NULL;
            }
        } break;
        case METADATA_FUNCTION_ARGUMENT: if (current->val.args.moveReg == regToLookFor) return current; break;
        case INSTRUCTION_ABS:break;
    }

    return fetchPreviousInstructionThatModifiesRegister(current->previous, regToLookFor);
}

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

        SimpleInstruction *movConst = NEW(SimpleInstruction);
        movConst->kind = INSTRUCTION_CONST;
        registerTrackerForBlock++;

        SimpleInstruction *sub = NEW(SimpleInstruction);
        sub->kind = INSTRUCTION_MINUS;
        registerTrackerForBlock++;
        appendInstruction(movConst, sub);
        addInstructionTemplate(peepholeTemplates, movConst, REMOVE_CONST_REGISTER_SUB, 2);
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

    {
        size_t registerTrackerForBlock = ANY + 1;

        SimpleInstruction *add = NEW(SimpleInstruction);
        add->kind = INSTRUCTION_ADD_CONST;
        registerTrackerForBlock++;

        //addInstructionTemplate(peepholeTemplates, add, CONST_ADD_TO_LEA, 1);
    }

    {
        size_t registerTrackerForBlock = ANY + 1;

        SimpleInstruction *add = NEW(SimpleInstruction);
        add->kind = INSTRUCTION_ADD;
        registerTrackerForBlock++;

        //addInstructionTemplate(peepholeTemplates, add, ADD_TO_LEA, 1);
    }

    {
        size_t registerTrackerForBlock = ANY + 1;

        SimpleInstruction *add = NEW(SimpleInstruction);
        add->kind = COMPLEX_MOVE_TEMPORARY_FROM_STACK;
        registerTrackerForBlock++;

        //addInstructionTemplate(peepholeTemplates, add, ALREADY_COMPLEX_MOVE_TEMPORARY_FROM_STACK, 1);
    }

    {
        size_t registerTrackerForBlock = ANY + 1;

        SimpleInstruction *add = NEW(SimpleInstruction);
        add->kind = INSTRUCTION_PUSH;
        registerTrackerForBlock++;

        //addInstructionTemplate(peepholeTemplates, add, POP_PUSH_STACK_PTR_LOAD, 1);
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
            Instructions *cons = instructionsIter;
            Instructions *add = skipToNextImportantInstruction(instructionsIter->next);
            if (cons->val.constant.temp == add->val.arithmetic2.source) {
                instructionHead = newInstruction();
                instructionHead->kind = INSTRUCTION_ADD_CONST;
                instructionHead->val.art2const.constant = cons->val.constant.value;
                instructionHead->val.art2const.temp = add->val.arithmetic2.dest;
                instructionHead->dataFlowEntry = mergeDataFlowEntries(cons->dataFlowEntry, add->dataFlowEntry);
                currentInstruction = instructionHead;
                templateApplied = true;
            }
        } break;
        case REMOVE_CONST_REGISTER_MUL: {
            Instructions *cons = instructionsIter;
            Instructions *mul = skipToNextImportantInstruction(instructionsIter->next);
            if (cons->val.constant.temp == mul->val.arithmetic2.source) {
                instructionHead = newInstruction();
                instructionHead->kind = INSTRUCTION_MUL_CONST;
                instructionHead->val.art2const.constant = cons->val.constant.value;
                instructionHead->val.art2const.temp = mul->val.arithmetic2.dest;
                instructionHead->dataFlowEntry = mergeDataFlowEntries(cons->dataFlowEntry, mul->dataFlowEntry);
                currentInstruction = instructionHead;
                templateApplied = true;
            }
        } break;
        case REMOVE_CONST_REGISTER_SUB: {
            Instructions *cons = instructionsIter;
            Instructions *sub = skipToNextImportantInstruction(instructionsIter->next);
            if (cons->val.constant.temp == sub->val.arithmetic2.source) {
                instructionHead = newInstruction();
                instructionHead->kind = INSTRUCTION_SUB_CONST;
                instructionHead->val.art2const.constant = cons->val.constant.value;
                instructionHead->val.art2const.temp = sub->val.arithmetic2.dest;
                instructionHead->dataFlowEntry = mergeDataFlowEntries(cons->dataFlowEntry, sub->dataFlowEntry);
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
                instructionHead->dataFlowEntry = instructionsIter->dataFlowEntry;
                currentInstruction = instructionHead;
                templateApplied = true;
            }
        } break;
        case CONST_ADD_TO_LEA: {
            instructionHead = newInstruction();
            instructionHead->kind = INSTRUCTION_LEA_ADD_CONST;
            instructionHead->val.art2const.constant = instructionsIter->val.art2const.constant;
            instructionHead->val.art2const.temp = instructionsIter->val.art2const.temp;
            instructionHead->dataFlowEntry = instructionsIter->dataFlowEntry;
            currentInstruction = instructionHead;
            templateApplied = true;
        } break;
        case ADD_TO_LEA: {
            instructionHead = newInstruction();
            instructionHead->kind = INSTRUCTION_LEA_ADD;
            instructionHead->val.arithmetic2.dest = instructionsIter->val.arithmetic2.dest;
            instructionHead->val.arithmetic2.source = instructionsIter->val.arithmetic2.source;
            instructionHead->dataFlowEntry = instructionsIter->dataFlowEntry;
            currentInstruction = instructionHead;
            templateApplied = true;
        } break;
        case ALREADY_COMPLEX_MOVE_TEMPORARY_FROM_STACK: {
            Instructions *instr = fetchPreviousInstructionThatModifiesRegister(instructionsIter, instructionsIter->val.tempFromStackScope.inputTemp);
            if (instr != NULL) {
                if (instr->kind == instructionsIter->kind) {
                    Instructions *matching = lookForDupFetches(instructionsIter, instructionsIter->next, instructionsIter->kind);
                    if (instr == matching) {
                        instructionHead = newInstruction();
                        instructionHead->kind = NOOP;
                        templateApplied = true;
                    }
                }
            }
        } break;
        case POP_PUSH_STACK_PTR_LOAD: {
            size_t temp = instructionsIter->val.tempToPush;

            Instructions *instr = NULL; //FIND INSTRUCTION THAT POPS TEMPORARY

            size_t amountOfPushesBetween = 0;
            size_t amountOfPopsBetween = 0;

            int stackOffset = amountOfPushesBetween - amountOfPopsBetween;

            if (true) { //Correct condition
                instructionHead = newInstruction();
                instructionHead->kind = INSTRUCTION_LOAD_STACK_PTR;
                instructionHead->val.loadStackPtr.offset = stackOffset * POINTER_SIZE;
                instructionHead->val.loadStackPtr.temp = instructionsIter->val.tempToPush;
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
            currentInstruction->next = toReplaceTo;
            instructionHead->previous = toReplaceFrom;
            toReplaceFrom->next = instructionHead;
            toReplaceTo->previous = currentInstruction;
        }

        instructionsChanged = true;
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
        case RUNTIME_ARRAY_BOUNDS_CHECK: { return instructions; } break;
        case RUNTIME_NEGATIVE_ALLOC: { return instructions; } break;
        case RUNTIME_NULLPTR_CHECK: { return instructions; } break;
        case RUNTIME_DIV_ZERO: { return instructions; } break;
        case INSTRUCTION_LEA_ADD:{ return instructions; } break;
        case INSTRUCTION_LEA_ADD_CONST:{ return instructions; } break;
        case INSTRUCTION_SUB_CONST:{ return instructions; } break;
        case NOOP:{ return skipToNextImportantInstruction(instructions->next); } break;
        case INSTRUCTION_LOAD_STACK_PTR: { return  instructions; } break;
        case METADATA_ACCESS_VARIABLE_START:{ return skipToNextImportantInstruction(instructions->next); } break;
        case METADATA_ACCESS_VARIABLE_END:{ return skipToNextImportantInstruction(instructions->next); } break;
    }
}

bool peephole(Instructions *instructions) {
    PeepholeTemplates *peepholeTemplates = generateRulesetsForSize();

    Instructions *iter = instructions;
    bool notEqual;
    instructionsChanged = false;

    int amountOfTimesIterPeepholed = 0;
    while (iter != NULL) {
    //    if (amountOfTimesIterPeepholed > 20) {
    //       amountOfTimesIterPeepholed = 0;
    //       iter = iter->next;
    //       continue;
    //    }
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

            //Apply template
            simpleInstructionIter = templateIter->simpleInstruction;
            realInstructionIter = iter;

            iter = applyTemplate(simpleInstructionIter, realInstructionIter, (size_t)templateIter->size, templateIter->apply);//, smallestReg);

            //The template was applied
            if (iter != realInstructionIter) {
                amountOfTimesIterPeepholed++;
            }

            templateIter = templateIter->next;
        }

        iter = iter->next;
    }

    return instructionsChanged;
}
