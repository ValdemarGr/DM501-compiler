//
// Created by valde on 4/23/19.
//

#include "peephole.h"

#define ANY 0

typedef struct Rule1 {
    size_t reg1;
} Rule1;

typedef struct Rule2 {
    size_t reg1;
    size_t reg2;
} Rule2;

typedef struct SimpleInstruction {
    InstructionKind kind;
    struct SimpleInstruction *next;
    union {
        Rule1 rule1;
        Rule2 rule2;
    } val;
} SimpleInstruction;

typedef struct PeepholeTemplates {
    SimpleInstruction *simpleInstruction;
    struct PeepholeTemplates *next;
} PeepholeTemplates;

void addInstructionTemplate(PeepholeTemplates *peepholeTemplates, SimpleInstruction *simpleInstruction) {
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

    PeepholeTemplates *newTemplates = (PeepholeTemplates*)(malloc(sizeof(PeepholeTemplates)));

    newTemplates->next = NULL;
    iter->next = newTemplates;
    newTemplates->simpleInstruction = simpleInstruction;
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

                addInstructionTemplate(peepholeTemplates, movConst);
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

                addInstructionTemplate(peepholeTemplates, movConst);
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

Instructions *peephole(Instructions *instructions) {
    int n = 2;

    PeepholeTemplates *peepholeTemplates = generateRulesetsForSize(n);

    Instructions *iter = instructions;

    while (iter != NULL) {
        PeepholeTemplates *templateIter = peepholeTemplates;

        while (templateIter != NULL) {
            SimpleInstruction *simpleInstructionIter = templateIter->simpleInstruction;
            Instructions *realInstructionIter = iter;

            while (simpleInstructionIter != NULL &&
            realInstructionIter != NULL &&
            simpleInstructionIter->kind == realInstructionIter->kind) {

                realInstructionIter = realInstructionIter->next;
                simpleInstructionIter = simpleInstructionIter->next;
            }

            simpleInstructionIter = templateIter->simpleInstruction;
            realInstructionIter = iter;

            if ()

            templateIter = templateIter->next;
        }

        iter = iter->next;
    }
}
