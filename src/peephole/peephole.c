//
// Created by valde on 4/23/19.
//

#include "peephole.h"
#include "../ast_to_ir/intermediate_representation.h"

Instructions *skipToNextImportantInstruction(Instructions *instructions);

void addInstructionTemplate(PeepholeTemplates *peepholeTemplates, SimpleInstruction *simpleInstruction, PeepholeApplyType apply) {
    PeepholeTemplates *iter = peepholeTemplates;

    while (iter != NULL) {

        if (iter->next == NULL || iter->simpleInstruction == NULL) {
            break;
        }

        iter = iter->next;
    }

    if (iter == NULL) {
        return;
    }

    if (iter->simpleInstruction == NULL) {
        iter->next = NULL;
        iter->simpleInstruction = simpleInstruction;
        iter->apply = apply;
    } else {
        PeepholeTemplates *newTemplates = (PeepholeTemplates*)(malloc(sizeof(PeepholeTemplates)));

        newTemplates->next = NULL;
        iter->next = newTemplates;
        newTemplates->simpleInstruction = simpleInstruction;
        newTemplates->apply = apply;
    }
}

void appendInstruction(SimpleInstruction *simpleInstruction, SimpleInstruction *new) {
    new->next = NULL;
    simpleInstruction->next = new;
}

void appendTemplates(PeepholeTemplates *peepholeTemplates, PeepholeTemplates *new) {
    new->next = NULL;
    peepholeTemplates->next = new;
}

PeepholeTemplates *generateRulesetsForSize(int n) {
    PeepholeTemplates *peepholeTemplates = (PeepholeTemplates*)(malloc(sizeof(PeepholeTemplates)));

    switch (n) {
        case 1: {


        } break;
        case 2: {
            {
                size_t registerTrackerForBlock = ANY + 1;

                SimpleInstruction *movConst = NEW(SimpleInstruction);
                movConst->kind = INSTRUCTION_CONST;
                movConst->val.rule1.reg1 = registerTrackerForBlock;
                registerTrackerForBlock++;

                SimpleInstruction *simpleAdd = NEW(SimpleInstruction);
                simpleAdd->kind = INSTRUCTION_ADD;
                simpleAdd->val.rule2.reg1 = registerTrackerForBlock - 1;
                simpleAdd->val.rule2.reg2 = registerTrackerForBlock;
                registerTrackerForBlock++;
                appendInstruction(movConst, simpleAdd);

                addInstructionTemplate(peepholeTemplates, movConst, REMOVE_CONST_REGISTER_ADD);
            }

            {
                size_t registerTrackerForBlock = ANY + 1;

                SimpleInstruction *movConst = NEW(SimpleInstruction);
                movConst->kind = INSTRUCTION_CONST;
                movConst->val.rule1.reg1 = registerTrackerForBlock;
                registerTrackerForBlock++;

                SimpleInstruction *simpleMul = NEW(SimpleInstruction);
                simpleMul->kind = INSTRUCTION_MUL;
                simpleMul->val.rule2.reg1 = registerTrackerForBlock - 1;
                simpleMul->val.rule2.reg2 = registerTrackerForBlock;
                registerTrackerForBlock++;
                appendInstruction(movConst, simpleMul);

                addInstructionTemplate(peepholeTemplates, movConst, REMOVE_CONST_REGISTER_MUL);
            }

        } break;
        case 3: {

        } break;
        case 4: {

        } break;
        case 5: {

        } break;
        case 6: {

        } break;
        case 7: {

        } break;
        case 8: {

        } break;
        case 9: {

        } break;
        case 10: {

        } break;
        case 11: {

        } break;
        case 12: {

        } break;
        default:break;
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
//Replacement patterns
void applyTemplate(SimpleInstruction *simpleHead, Instructions *instrHead, size_t n, PeepholeApplyType apply) {//, size_t min) {
    SimpleInstruction *simpleIter = simpleHead;
    Instructions *instructionsIter = instrHead;

    Instructions *instructionHead = newInstruction();
    Instructions *currentInstruction = instructionHead;

    switch (apply) {
        case REMOVE_CONST_REGISTER_ADD: {
            instructionHead->kind = INSTRUCTION_ADD_CONST;
            instructionHead->val.art2const.constant = instructionsIter->val.constant.value;
            instructionsIter =  skipToNextImportantInstruction(instructionsIter->next);
            instructionHead->val.art2const.temp = instructionsIter->val.arithmetic2.dest;
        } break;
        case REMOVE_CONST_REGISTER_MUL: {
            instructionHead->kind = INSTRUCTION_MUL_CONST;
            instructionHead->val.art2const.constant = instrHead->val.constant.value;
            instructionsIter = skipToNextImportantInstruction(instructionsIter->next);
            instructionHead->val.art2const.temp = instructionsIter->val.arithmetic2.dest;
        } break;
    }

    //Replace entire context
    Instructions *toReplaceFrom = instrHead->previous;
    Instructions *toReplaceTo = instrHead;

    for (int i = 0; i < n; ++i) {
        toReplaceTo = toReplaceTo->next;
    }

    toReplaceFrom->next = instructionHead;
    currentInstruction->next = toReplaceTo->next;
}

bool checkForInstructionEquality(SimpleInstruction *simpleHead, Instructions *instrHead) {
    if (simpleHead->kind != instrHead->kind) {
        return false;
    }

    //Now do register equality
    switch (instrHead->kind) {
        case INSTRUCTION_ADD: {
            int instrDiff = (int)instrHead->val.arithmetic2.dest -
                    (int)instrHead->val.arithmetic2.source;
            int simpleDiff = (int)simpleHead->val.rule2.reg2 -
                    (int)simpleHead->val.rule2.reg1;
            if (simpleDiff != instrDiff) {
                return false;
            }
        } break;
        case INSTRUCTION_MINUS: {

        } break;
        case INSTRUCTION_MUL: {

        } break;
        case INSTRUCTION_DIV: {

        } break;
        case INSTRUCTION_CONST: {

        } break;
        case INSTRUCTION_PROGRAM_BEGIN: {

        } break;
        case INSTRUCTION_FUNCTION_LABEL: {

        } break;
        case INSTRUCTION_VAR: {

        } break;
        case INSTRUCTION_FUNCTION_END: {

        } break;
        case INSTRUCTION_RETURN: {

        } break;
        case INSTRUCTION_WRITE: {

        } break;
        case INSTRUCTION_AND: {

        } break;
        case INSTRUCTION_OR: {

        } break;
        case INSTRUCTION_PUSH: {

        } break;
        case INSTRUCTION_POP: {

        } break;
        case INSTRUCTION_NEGATE: {

        } break;
        case INSTRUCTION_ABS: {

        } break;
        case INSTRUCTION_FUNCTION_CALL: {

        } break;
        case INSTRUCTION_RIGHT_SHIFT: {

        } break;
        case INSTRUCTION_XOR: {

        } break;
        case INSTRUCTION_COPY: {

        } break;
        case INSTRUCTION_CMP: {

        } break;
        case INSTRUCTION_LABEL: {

        } break;
        case INSTRUCTION_JE: {

        } break;
        case INSTRUCTION_JMP: {

        } break;
        case INSTRUCTION_MOVE: {

        } break;
        case COMPLEX_ALLOCATE: {

        } break;
        case COMPLEX_ALLOCATE_END: {

        } break;
        case COMPLEX_CONSTRAIN_BOOLEAN: {

        } break;
        case COMPLEX_SAVE_STATIC_LINK: {

        } break;
        case COMPLEX_RESTORE_STATIC_LINK: {

        } break;
        case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME: {

        } break;
        case METADATA_BEGIN_BODY_BLOCK: {

        } break;
        case METADATA_END_BODY_BLOCK: {

        } break;
        case METADATA_FUNCTION_ARGUMENT: {

        } break;
        case METADATA_CREATE_MAIN: {

        } break;
        case METADATA_BEGIN_ARITHMETIC_EVALUATION: {

        } break;
        case METADATA_END_ARITHMETIC_EVALUATION: {

        } break;
        case INSTRUCTION_ADD_CONST: {

        } break;
        case INSTRUCTION_MUL_CONST: {

        } break;
    }

    return true;
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
        case METADATA_END_BODY_BLOCK: { return skipToNextImportantInstruction(instructions->next);} break;
        case METADATA_FUNCTION_ARGUMENT: { return skipToNextImportantInstruction(instructions->next);} break;
        case METADATA_CREATE_MAIN: { return skipToNextImportantInstruction(instructions->next);} break;
        case METADATA_BEGIN_ARITHMETIC_EVALUATION: { return skipToNextImportantInstruction(instructions->next);} break;
        case METADATA_END_ARITHMETIC_EVALUATION: {return skipToNextImportantInstruction(instructions->next); } break;
    }
}

void peephole(Instructions *instructions) {
    int n = 2;

    PeepholeTemplates *peepholeTemplates = generateRulesetsForSize(n);

    Instructions *iter = instructions;

    while (iter != NULL) {
        PeepholeTemplates *templateIter = peepholeTemplates;

        while (templateIter != NULL) {
            SimpleInstruction *simpleInstructionIter = templateIter->simpleInstruction;
            Instructions *realInstructionIter = iter;

            //Test equality of templates, this includes register layout
            while (simpleInstructionIter != NULL &&
            realInstructionIter != NULL &&
            checkForInstructionEquality(simpleInstructionIter, realInstructionIter)) {

                realInstructionIter = skipToNextImportantInstruction(realInstructionIter->next);
                simpleInstructionIter = simpleInstructionIter->next;
            }

            //Null test
            if (simpleInstructionIter != NULL) {
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

            while (simpleInstructionIter != NULL &&
                   realInstructionIter != NULL) {

                applyTemplate(simpleInstructionIter, realInstructionIter, (size_t)n, templateIter->apply);//, smallestReg);

                realInstructionIter = skipToNextImportantInstruction(realInstructionIter->next);
                simpleInstructionIter = simpleInstructionIter->next;
            }

            //We reset iter if we do a peephole
            iter = iter->previous;

            templateIter = templateIter->next;
        }

        iter = iter->next;
    }
}
