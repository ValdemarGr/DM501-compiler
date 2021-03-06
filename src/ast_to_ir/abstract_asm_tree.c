#include "abstract_asm_tree.h"
#include "intermediate_representation.h"
#include "../ast/tree.h"
#include "../symbol/symbol.h"

Instructions *instructionHead = NULL;
Instructions *currentInstruction = NULL;
bool mainCreated = false;
size_t ifCounter = 0;
size_t whileCounter = 0;
size_t returnReg = 0;
bool inClassContextCurrent = false;
int classGenericCount = 0;
bool inLambdaContext = false;
bool currentlyInConstructorContext = false;
int lambdaDefineScope = 0;
int lambdaArgCount = 0;
int staticLinkDepth = -1;
char *className;

ConstMap *lambdaEncounterd = NULL;

//If the context stack contains something we need to apply the instructions in the current context
//static Stack *contextStack = NULL;
extern size_t currentTemporary;
extern size_t maxTemporary;

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

typedef struct VDLResult {
    VarDelList *head;
    VarDelList *tail;
} VDLResult;

VDLResult *generateVDLForClassDecls(VarDelList *currentTail, Declaration *declaration) {
    VDLResult *r = NEW(VDLResult);

    if (declaration == NULL) {
        r->head = currentTail;
        r->tail = currentTail;
        return r;
    }

    switch (declaration->kind) {
        case declVarK: {
            if (currentTail == NULL) {
                currentTail = NEW(VarDelList);
                currentTail->identifier = declaration->val.varD.id;
                currentTail->type = declaration->val.varD.type;
                currentTail->next = NULL;
                r->head = currentTail;
                r->tail = currentTail;
            } else {
                VarDelList *next = NEW(VarDelList);
                next->identifier = declaration->val.varD.id;
                next->type = declaration->val.varD.type;
                next->next = NULL;
                currentTail->next = next;
                r->tail = next;
                r->head = currentTail;
            }
        } break;
        case declVarsK: {
            //For this
            VarDelList *res = currentTail;
            if (res == NULL) {
                res = NEW(VarDelList);
                res->identifier = declaration->val.varsD.var->val.varD.id;
                res->type = declaration->val.varsD.var->val.varD.type;
                res->next = NULL;
                r->head = res;
                r->tail = res;
            } else {
                VarDelList *next = NEW(VarDelList);
                next->identifier = declaration->val.varsD.var->val.varD.id;
                next->type = declaration->val.varsD.var->val.varD.type;
                next->next = NULL;
                res->next = next;
                res = next;
                r->head = currentTail;
                r->tail = next;
            }

            //For the next
            r->tail = generateVDLForClassDecls(res, declaration->val.varsD.next)->tail;
        } break;
        case declValK: {
            if (currentTail == NULL) {
                currentTail = NEW(VarDelList);
                currentTail->identifier = declaration->val.valD.id;
                currentTail->type = declaration->val.valD.tpe;
                currentTail->next = NULL;
                r->head = currentTail;
                r->tail = currentTail;
            } else {
                VarDelList *next = NEW(VarDelList);
                next->identifier = declaration->val.valD.id;
                next->type = declaration->val.valD.tpe;
                next->next = NULL;
                currentTail->next = next;
                r->tail = next;
                r->head = currentTail;
            }
        } break;
    }

    return r;
}

size_t generateInstructionsForVariableAccessInternal(Variable *variable, SymbolTable *symbolTable) {
    switch (variable->kind) {
        case varIdK: {
            SYMBOL *symbol = getSymbol(symbolTable, variable->val.idD.id);

            //Check how far up the scope stack we need to look for the variable
            size_t frameStackDistanceToVariable = symbolTable->distanceFromRoot - symbol->distanceFromRoot;

            size_t offset = POINTER_SIZE;

            if ((inLambdaContext || currentlyInConstructorContext) && inClassContextCurrent && symbol->distanceFromRoot == lambdaDefineScope) {
                Instructions *debug = newInstruction();
                debug->kind = METADATA_DEBUG_INFO;
                debug->val.debugInfo = "CLASS LOAD";
                appendInstructions(debug);

                //We are accessing a class value from lambda (capture)
                //Fetch the class pointer which is the last pushed argument
                Instructions *ptrAccess = newInstruction();
                ptrAccess->kind = COMPLEX_MOVE_TEMPORARY_FROM_STACK;
                ptrAccess->val.tempFromStack.offset = offset * lambdaArgCount;
                ptrAccess->val.tempFromStack.inputTemp = currentTemporary;
                appendInstructions(ptrAccess);
                size_t classPtrTemp = currentTemporary;
                currentTemporary++;

                Instructions *constOffset = newInstruction();
                constOffset->kind = INSTRUCTION_CONST;
                constOffset->val.constant.value = POINTER_SIZE * ((int)symbol->uniqueIdForScope);
                constOffset->val.constant.temp = currentTemporary;
                size_t constOffsetTemp = currentTemporary;
                appendInstructions(constOffset);
                currentTemporary++;

                //Now we must find the member we are accessing
                Instructions *memberAccess = newInstruction();
                memberAccess->kind = COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET;
                memberAccess->val.dereferenceOffset.ptrTemp = classPtrTemp;
                memberAccess->val.dereferenceOffset.offsetTemp = constOffsetTemp;
                memberAccess->val.dereferenceOffset.size = 1;
                //classPtrTemp = currentTemporary;
                appendInstructions(memberAccess);
                //currentTemporary++;

                return classPtrTemp;
            } if (frameStackDistanceToVariable == 0) {
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
                ptrAccess->val.tempFromStackScope.inputTemp = currentTemporary++;
                ptrAccess->val.tempFromStackScope.scopeToFindFrame = symbol->distanceFromRoot;
                ptrAccess->val.tempFromStackScope.intermediate = currentTemporary++;
                ptrAccess->val.tempFromStackScope.intermediate2 = currentTemporary++;
                appendInstructions(ptrAccess);
                return currentTemporary - 3;
            }
        } break;
        case arrayIndexK: {
            size_t accessTemp = generateInstructionsForVariableAccessInternal(variable->val.arrayIndexD.var, symbolTable);

            Instructions *check = newInstruction();
            check->kind = RUNTIME_NULLPTR_CHECK;
            check->val.divZeroTemp = accessTemp;
            appendInstructions(check);

            size_t exprTemp = generateInstructionsForExpression(variable->val.arrayIndexD.idx, symbolTable);

            Instructions *arrayBounds = newInstruction();
            arrayBounds->kind = RUNTIME_ARRAY_BOUNDS_CHECK;
            arrayBounds->val.arrayBounds.arrPtr = accessTemp;
            arrayBounds->val.arrayBounds.exprTemp = exprTemp;
            appendInstructions(arrayBounds);

            Instructions *tpeConst = newInstruction();
            tpeConst->kind = INSTRUCTION_CONST;
            tpeConst->val.constant.temp = currentTemporary;
            tpeConst->val.constant.value = POINTER_SIZE;
            size_t typeSizeTemp = currentTemporary;
            appendInstructions(tpeConst);
            currentTemporary++;

            Instructions *mulOffset = newInstruction();
            mulOffset->kind = INSTRUCTION_MUL;
            mulOffset->val.arithmetic2.source = typeSizeTemp;
            mulOffset->val.arithmetic2.dest = exprTemp;
            appendInstructions(mulOffset);

            Instructions *ptrAccess = newInstruction();
            ptrAccess->kind = COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET;
            ptrAccess->val.dereferenceOffset.ptrTemp = accessTemp;
            ptrAccess->val.dereferenceOffset.offsetTemp = exprTemp;
            ptrAccess->val.dereferenceOffset.size = 1;
            //accessTemp = currentTemporary;
            appendInstructions(ptrAccess);
            //currentTemporary++;
            return accessTemp;
        } break;
        case recordLookupK: {
            size_t accessTemp = generateInstructionsForVariableAccessInternal(variable->val.recordLookupD.var, symbolTable);

            Instructions *check = newInstruction();
            check->kind = RUNTIME_NULLPTR_CHECK;
            check->val.divZeroTemp = accessTemp;
            appendInstructions(check);

            Type *unwrappedType = unwrapTypedef(unwrapVariable(variable->val.recordLookupD.var, symbolTable), symbolTable, NULL, false);

            VarDelList *varDelList = NULL;
            size_t sizeAccumulator = 0;

            if (unwrappedType->kind == typeRecordK) {
                varDelList = unwrappedType->val.recordType.types;

                //Todo classes

                while (strcmp(varDelList->identifier, variable->val.recordLookupD.id) != 0) {
                    sizeAccumulator = sizeAccumulator + POINTER_SIZE;
                    varDelList = varDelList->next;
                }
            } else {
                //Must be class or genericly type constrianed class
                SYMBOL *classSym;
                if (unwrappedType->kind == typeClassK) {
                    classSym = getSymbol(symbolTable, unwrappedType->val.typeClass.classId);
                } else {
                    //Generic with constraint, get sym by looking for generic name in this class's generics
                    char *typeConstriantClass = unwrappedType->val.typeGeneric.subType;
                    classSym = getSymbol(symbolTable, typeConstriantClass);
                }

                SymbolTable *bodyTable = classSym->value->val.typeClassD.tableForClassBody;

                sizeAccumulator = (getSymbol(bodyTable, variable->val.recordLookupD.id)->uniqueIdForScope) * POINTER_SIZE;
/*
                DeclarationList *declarationList = classSym->value->val.typeClassD.declarationList;

                VarDelList *vdlIter = NULL;

                //Grab types, allowed decls are val and var
                while (declarationList != NULL) {
                    VDLResult *r = generateVDLForClassDecls(vdlIter, declarationList->declaration);

                    if (varDelList == NULL) {
                        varDelList = r->head;
                    }

                    vdlIter = r->tail;

                    declarationList = declarationList->next;
                }*/
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
            ptrAccess->val.dereferenceOffset.size = 1;
            //accessTemp = currentTemporary;
            appendInstructions(ptrAccess);
            //currentTemporary++;
            return accessTemp;
        } break;
    }
}

char *generateVariableUniqueId(Variable* variable, SymbolTable *symbolTable) {
    switch (variable->kind) {
        case varIdK: {
            SYMBOL *sym = getSymbol(symbolTable, variable->val.idD.id);
            char *toReturnId = malloc(sizeof(char) * (strlen(variable->val.idD.id) + 10));

            sprintf(toReturnId, "%s%i", variable->val.idD.id, (int)sym->distanceFromRoot);

            return toReturnId;
        } break;
        case arrayIndexK: {
            return NULL;
        } break;
        case recordLookupK: {
            char *inner = generateVariableUniqueId(variable->val.recordLookupD.var, symbolTable);

            if (inner == NULL) return NULL;

            char *newBuf = malloc(sizeof(char) * (strlen(inner) + strlen(variable->val.recordLookupD.id)));

            sprintf(newBuf, "%s.%s", inner, variable->val.recordLookupD.id);

            return newBuf;
        } break;
    }

    return "";
}

size_t generateInstructionsForVariableAccess(Variable *variable, SymbolTable *symbolTable) {
    char *accessId = generateVariableUniqueId(variable, symbolTable);

    Instructions *metadataBeginAccess;
    if (accessId != NULL) {
        metadataBeginAccess = newInstruction();
        metadataBeginAccess->kind = METADATA_ACCESS_VARIABLE_START;
        metadataBeginAccess->val.varAccess.accessId = accessId;
        appendInstructions(metadataBeginAccess);
    }

    size_t toReturn = generateInstructionsForVariableAccessInternal(variable, symbolTable);

    if (accessId != NULL) {
        metadataBeginAccess->val.varAccess.temp = toReturn;

        Instructions *metadataAccessEnd;
        metadataAccessEnd = newInstruction();
        metadataAccessEnd->kind = METADATA_ACCESS_VARIABLE_END;
        metadataAccessEnd->val.varAccess.accessId = accessId;
        metadataAccessEnd->val.varAccess.temp = toReturn;
        appendInstructions(metadataAccessEnd);
    }

    return toReturn;
}

void generateInstructionsForVariableSave(Variable *variable, SymbolTable *symbolTable, size_t tempToSave, bool forArrayLen) {
    switch (variable->kind) {
        case varIdK: {
            SYMBOL *symbol = getSymbol(symbolTable, variable->val.idD.id);

            //Check how far up the scope stack we need to look for the variable
            size_t frameStackDistanceToVariable = symbolTable->distanceFromRoot - symbol->distanceFromRoot;

            size_t offset = POINTER_SIZE;

            if ((inLambdaContext || currentlyInConstructorContext) && inClassContextCurrent && symbol->distanceFromRoot == lambdaDefineScope) {
                Instructions *debug = newInstruction();
                debug->kind = METADATA_DEBUG_INFO;
                debug->val.debugInfo = "CLASS LOAD";
                appendInstructions(debug);

                //We are accessing a class value from lambda (capture)
                //Fetch the class pointer which is the last pushed argument
                Instructions *ptrAccess = newInstruction();
                ptrAccess->kind = COMPLEX_MOVE_TEMPORARY_FROM_STACK;
                ptrAccess->val.tempFromStack.offset = offset * lambdaArgCount;
                ptrAccess->val.tempFromStack.inputTemp = currentTemporary;
                appendInstructions(ptrAccess);
                size_t classPtrTemp = currentTemporary;
                currentTemporary++;

                Instructions *constOffset = newInstruction();
                constOffset->kind = INSTRUCTION_CONST;
                constOffset->val.constant.value = POINTER_SIZE * ((int)symbol->uniqueIdForScope);
                constOffset->val.constant.temp = currentTemporary;
                size_t constOffsetTemp = currentTemporary;
                appendInstructions(constOffset);
                currentTemporary++;

                Instructions *memberSave = newInstruction();
                memberSave->kind = INSTRUCTION_MOVE_TO_OFFSET;
                memberSave->val.moveToOffset.ptrTemp = classPtrTemp;
                memberSave->val.moveToOffset.offsetTemp = constOffsetTemp;
                memberSave->val.moveToOffset.tempToMove = tempToSave;
                appendInstructions(memberSave);
            } else if (frameStackDistanceToVariable == 0) {
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
                ptrAccess->val.tempFromStackScope.intermediate = currentTemporary++;
                ptrAccess->val.tempFromStackScope.intermediate2 = currentTemporary++;
                appendInstructions(ptrAccess);
            }
        } break;
        case arrayIndexK: {
            size_t accessTemp = generateInstructionsForVariableAccess(variable->val.arrayIndexD.var, symbolTable);

            Instructions *check = newInstruction();
            check->kind = RUNTIME_NULLPTR_CHECK;
            check->val.divZeroTemp = accessTemp;
            appendInstructions(check);

            size_t exprTemp = generateInstructionsForExpression(variable->val.arrayIndexD.idx, symbolTable);

            Instructions *arrayBounds = newInstruction();
            arrayBounds->kind = RUNTIME_ARRAY_BOUNDS_CHECK;
            arrayBounds->val.arrayBounds.arrPtr = accessTemp;
            arrayBounds->val.arrayBounds.exprTemp = exprTemp;
            appendInstructions(arrayBounds);

            Instructions *constPtrSize = newInstruction();
            constPtrSize->kind = INSTRUCTION_CONST;
            constPtrSize->val.constant.value = POINTER_SIZE;
            constPtrSize->val.constant.temp= currentTemporary;
            appendInstructions(constPtrSize);
            size_t ptrReg = currentTemporary;
            currentTemporary++;

            Instructions *mulOffset = newInstruction();
            mulOffset->kind = INSTRUCTION_MUL;
            mulOffset->val.arithmetic2.source = ptrReg;
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

            Instructions *check = newInstruction();
            check->kind = RUNTIME_NULLPTR_CHECK;
            check->val.divZeroTemp = accessTemp;
            appendInstructions(check);

            Type *unwrappedType = unwrapTypedef(unwrapVariable(variable->val.recordLookupD.var, symbolTable), symbolTable, NULL, true);

            VarDelList *varDelList = NULL;

            size_t sizeAccumulator = 0;

            if (unwrappedType->kind == typeRecordK) {
                varDelList = unwrappedType->val.recordType.types;

                //Todo classes

                while (strcmp(varDelList->identifier, variable->val.recordLookupD.id) != 0) {
                    sizeAccumulator = sizeAccumulator + POINTER_SIZE;
                    varDelList = varDelList->next;
                }
            } else {
                //Must be class or genericly type constrianed class
                SYMBOL *classSym;
                if (unwrappedType->kind == typeClassK) {
                    classSym = getSymbol(symbolTable, unwrappedType->val.typeClass.classId);
                } else {
                    //Generic with constraint, get sym by looking for generic name in this class's generics
                    char *typeConstriantClass = unwrappedType->val.typeGeneric.subType;
                    classSym = getSymbol(symbolTable, typeConstriantClass);
                }

                SymbolTable *bodyTable = classSym->value->val.typeClassD.tableForClassBody;

                sizeAccumulator = (getSymbol(bodyTable, variable->val.recordLookupD.id)->uniqueIdForScope) * POINTER_SIZE;
/*
                DeclarationList *declarationList = classSym->value->val.typeClassD.declarationList;

                VarDelList *vdlIter = NULL;

                //Grab types, allowed decls are val and var
                while (declarationList != NULL) {
                    VDLResult *r = generateVDLForClassDecls(vdlIter, declarationList->declaration);

                    if (varDelList == NULL) {
                        varDelList = r->head;
                    }

                    vdlIter = r->tail;

                    declarationList = declarationList->next;
                }*/
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
        case charK: {
            Instructions *num = newInstruction();
            num->kind = INSTRUCTION_CONST;
            num->val.constant.value = (int)(*term->val.charD.c);
            num->val.constant.temp = currentTemporary;
            appendInstructions(num);
            currentTemporary++;
            return currentTemporary - 1;
        } break;
        case variableK: {
            return generateInstructionsForVariableAccess(term->val.variableD.var, symbolTable);
        } break;
        case functionCallK: {
            //Give arguments on stack
            //For each expression argument, evaluate it and push it to the stack
            SYMBOL *symbol = getSymbol(symbolTable, term->val.functionCallD.functionId);

            /*
            if (symbol->distanceFromRoot == symbolTable->distanceFromRoot + 1) {
                //We need to save the current static link pointer
                Instructions *push = newInstruction();
                push->kind = COMPLEX_SAVE_STATIC_LINK;
                push->val.pushPopStaticLink.staticLinkDepth = symbol->distanceFromRoot;
                push->val.pushPopStaticLink.temporary = currentTemporary;
                appendInstructions(push);
                currentTemporary++;
            }*/

            //First we prepare if we are lambda
            size_t functionToCall = 0;
            //We either want to directly call or invoke a lambda function based on the type of our symbol
            if (symbol->value->kind == typeFunctionK && symbol->value->val.typeFunctionD.isLambda) {
                //This means we actually have to load the id as a variable
                //Dummy variable for access
                Variable *fvar = NEW(Variable);
                fvar->val.idD.id = term->val.functionCallD.functionId;
                fvar->kind = varIdK;

                functionToCall = generateInstructionsForVariableAccess(fvar, symbolTable);
            }

            Instructions *saveall = newInstruction();
            saveall->kind = COMPLEX_SAVE_ALL;
            //appendInstructions(saveall);

            ExpressionList *expressionList = term->val.functionCallD.expressionList;

            int toAdd = 0;

            while (expressionList != NULL) {

                //Evaluate
                size_t temporaryForArgument = generateInstructionsForExpression(expressionList->expression, symbolTable);

                //Then push
                Instructions *push = newInstruction();
                push->kind = INSTRUCTION_PUSH;
                push->val.tempToPush = temporaryForArgument;
                appendInstructions(push);

                toAdd++;
                expressionList = expressionList->next;
            }

            Instructions *call;

            if (symbol->value->kind == typeFunctionK && symbol->value->val.typeFunctionD.isLambda) {

                call = newInstruction();
                call->kind = INSTRUCTION_REGISTER_CALL;
                call->val.callRegister = functionToCall;
                appendInstructions(call);
            }  else if (symbol->value->kind == typeK && symbol->value->val.typeD.tpe->kind) {
                //Pseudo var
                Variable *tmpVar = NEW(Variable);
                tmpVar->kind = varIdK;
                tmpVar->val.idD.id = symbol->name;

                //Get variable with function ptr
                size_t fncPtrTemp = generateInstructionsForVariableAccess(tmpVar, symbolTable);

                Instructions *check = newInstruction();
                check->kind = RUNTIME_NULLPTR_CHECK;
                check->val.divZeroTemp = fncPtrTemp;
                appendInstructions(check);

                Instructions *captureOffset = newInstruction();
                captureOffset->kind = INSTRUCTION_CONST;
                captureOffset->val.constant.value = POINTER_SIZE;
                captureOffset->val.constant.temp = currentTemporary;
                size_t captureTemp = currentTemporary;
                appendInstructions(captureOffset);
                currentTemporary++;

                //Deref an extra time, lambda's are always double dereferenced
                Instructions *capturePtr = newInstruction();
                capturePtr->kind = COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET;
                capturePtr->val.dereferenceOffset.ptrTemp = fncPtrTemp;
                capturePtr->val.dereferenceOffset.offsetTemp = captureTemp;
                capturePtr->val.dereferenceOffset.size = 1;
                //fncPtrTemp = currentTemporary;
                appendInstructions(capturePtr);
                //currentTemporary++;

                Instructions *debug = newInstruction();
                debug->kind = METADATA_DEBUG_INFO;
                debug->val.debugInfo = "CAPTURE PUSH";
                appendInstructions(debug);

                //Push capture pointer
                Instructions *push = newInstruction();
                push->kind = INSTRUCTION_PUSH;
                push->val.tempToPush = fncPtrTemp;
                appendInstructions(push);
                toAdd++;

                fncPtrTemp = generateInstructionsForVariableAccess(tmpVar, symbolTable);

                //Off by one since we have arr of ptrs and first is len
                Instructions *num = newInstruction();
                num->kind = INSTRUCTION_CONST;
                num->val.constant.value = 0;
                num->val.constant.temp = currentTemporary;
                size_t numTemp = currentTemporary;
                appendInstructions(num);
                currentTemporary++;

                //Deref an extra time, lambda's are always double dereferenced
                Instructions *ptrAccess = newInstruction();
                ptrAccess->kind = COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET;
                ptrAccess->val.dereferenceOffset.ptrTemp = fncPtrTemp;
                ptrAccess->val.dereferenceOffset.offsetTemp = numTemp;
                ptrAccess->val.dereferenceOffset.size = 1;
                //fncPtrTemp = currentTemporary;
                appendInstructions(ptrAccess);
                //currentTemporary++;

                //Then call
                call = newInstruction();
                call->kind = INSTRUCTION_REGISTER_CALL;
                call->val.callRegister = fncPtrTemp;
                appendInstructions(call);
            } else {
                //Append depdth to name
                char *baseName = term->val.functionCallD.functionId;
                int extra = 16;
                char *buf = (char*)malloc(sizeof(char) * (strlen(baseName) + extra));

                sprintf(buf, "%s__%i", baseName, (int)symbol->distanceFromRoot);

                call = newInstruction();
                call->kind = INSTRUCTION_FUNCTION_CALL;
                call->val.functionCall.function = buf;
                call->val.functionCall.restore = NULL;
                call->val.functionCall.save = NULL;
                appendInstructions(call);
            }

            Instructions *sp = newInstruction();
            sp->kind = INSTRUCTION_ADD_STACK_PTR;
            sp->val.toAddStackPtr = toAdd * POINTER_SIZE;
            appendInstructions(sp);

            Instructions *restoreall = newInstruction();
            restoreall->kind = COMPLEX_RESTORE_ALL;
            //appendInstructions(restoreall);

            call->val.functionCall.restore = restoreall;
            call->val.functionCall.save = saveall;

            //Return value is in rax

            /*
            //We have the return value on the stack
            Instructions *pop = newInstruction();
            pop->kind = INSTRUCTION_POP;
            pop->val.tempToPopInto = currentTemporary;
            appendInstructions(pop);
            currentTemporary++;*/

            //We need to restore the static link
            Instructions *popLink = newInstruction();
            popLink->kind = COMPLEX_RESTORE_STATIC_LINK;
            popLink->val.pushPopStaticLink.staticLinkDepth = symbolTable->distanceFromRoot;
            popLink->val.pushPopStaticLink.temporary = currentTemporary;
            appendInstructions(popLink);
            currentTemporary++;

            Instructions *cpy = newInstruction();
            cpy->kind = INSTRUCTION_COPY;
            cpy->val.arithmetic2.source = 0;
            cpy->val.arithmetic2.dest = currentTemporary;
            appendInstructions(cpy);
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
            //TODO DO this manually or MMX SSSE3 PABSD OR USE PROVIDED METHOD
            // mask = n >> (sizeof(int) * bitsof(char) - 1)
            // (mask + n)^mask
            Type *e = unwrapTypedef(evaluateExpressionType(term->val.absD.expression, symbolTable), symbolTable, NULL, true);
            size_t tempToAbsOn = generateInstructionsForExpression(term->val.absD.expression, symbolTable);

            if (e->kind == typeArrayK) {
                Instructions *num = newInstruction();
                num->kind = INSTRUCTION_CONST;
                num->val.constant.value = - POINTER_SIZE;
                num->val.constant.temp = currentTemporary;
                appendInstructions(num);
                currentTemporary++;

                Instructions *check = newInstruction();
                check->kind = RUNTIME_NULLPTR_CHECK;
                check->val.divZeroTemp = tempToAbsOn;
                appendInstructions(check);

                Instructions *ptrAccess = newInstruction();
                ptrAccess->kind = COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET;
                ptrAccess->val.dereferenceOffset.ptrTemp = tempToAbsOn;
                ptrAccess->val.dereferenceOffset.offsetTemp = currentTemporary - 1;
                ptrAccess->val.dereferenceOffset.size = 1;
                //size_t resultTemp = currentTemporary;
                appendInstructions(ptrAccess);
                //currentTemporary++;
                //return resultTemp;
                return tempToAbsOn;
            } else {
                Instructions *abs = newInstruction();
                abs->kind = COMPLEX_ABS_VALUE;
                abs->val.arithmetic2.source = tempToAbsOn;
                abs->val.arithmetic2.dest = currentTemporary++;
                appendInstructions(abs);
                return currentTemporary - 1;
/*
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

                return currentTemporary - 1;*/
            }
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
            char *lamPrefix = "lambda_";
            char *buf = (char*)malloc(sizeof(char) * (strlen(lamPrefix) + 10));
            sprintf(buf, "lambda_%i", term->val.lambdaD.lambda->id);

            inLambdaContext = true;
            lambdaDefineScope = (int)symbolTable->distanceFromRoot;
            //BIND THE LAMBDA BY LOADING FROM RIP
            Instructions *lambdaLoad = newInstruction();
            lambdaLoad->kind = COMPLEX_RIP_LAMBDA_LOAD;
            lambdaLoad->val.lambdaLoad.temporary = currentTemporary;
            lambdaLoad->val.lambdaLoad.lambdaGlobalName = buf;
            appendInstructions(lambdaLoad);
            size_t ripLoad = currentTemporary;
            currentTemporary++;

            size_t arrayTemp;
            //First we must create an array to store the loaded lambda
            {
                /*VarDelList *args = term->val.lambdaD.lambda->declarationList;

                size_t argc = 1; //First spot is for lambda ptr

                while (args != NULL) {
                    argc++;
                    args = args->next;
                }

                Instructions *argcIns = newInstruction();
                argcIns->kind = INSTRUCTION_CONST;
                argcIns->val.constant.value = (int)argc;
                argcIns->val.constant.temp= currentTemporary;
                size_t argcInsTemp = currentTemporary;
                appendInstructions(argcIns);
                currentTemporary++;

                Instructions *constOne = newInstruction();
                constOne->kind = INSTRUCTION_CONST;
                constOne->val.constant.value = 1;
                constOne->val.constant.temp= currentTemporary;
                size_t constOneTemp = currentTemporary;
                appendInstructions(constOne);
                currentTemporary++;

                //Reserve space for size
                Instructions *add = newInstruction();
                add->kind = INSTRUCTION_ADD;
                add->val.arithmetic2.source = constOneTemp;
                add->val.arithmetic2.dest = argcInsTemp;
                appendInstructions(add);*/

                Instructions *const1 = newInstruction();
                const1->kind = INSTRUCTION_CONST;
                const1->val.constant.value = 1;
                const1->val.constant.temp= currentTemporary;
                size_t const1Temp = currentTemporary;
                appendInstructions(const1);
                currentTemporary++;

                Instructions *ret = newInstruction();
                ret->kind = COMPLEX_ALLOCATE;
                ret->val.allocate.timesTemp = const1Temp;
                ret->val.allocate.eleSize = POINTER_SIZE;
                ret->val.allocate.intermediate = currentTemporary++;
                ret->val.allocate.allocationType = ALLOC_LAMBDA;
                size_t allocPtrTemp = 0;
                arrayTemp = allocPtrTemp;
                appendInstructions(ret);


                //Instructions for getting getting the address we need to move the pointer to
                {
                    Instructions *constZero = newInstruction();
                    constZero->kind = INSTRUCTION_CONST;
                    constZero->val.constant.value = 0;
                    constZero->val.constant.temp= currentTemporary;
                    size_t exprTemp = currentTemporary;
                    appendInstructions(constZero);
                    currentTemporary++;

                    Instructions *ptrAccess = newInstruction();
                    ptrAccess->kind = INSTRUCTION_MOVE_TO_OFFSET;
                    ptrAccess->val.moveToOffset.ptrTemp = allocPtrTemp;
                    ptrAccess->val.moveToOffset.offsetTemp = exprTemp;
                    ptrAccess->val.moveToOffset.tempToMove = ripLoad;
                    appendInstructions(ptrAccess);
                }

                //Instructions *endAlloc = newInstruction();
                //endAlloc->kind = COMPLEX_ALLOCATE_END;
                //appendInstructions(endAlloc);
            }

            if (get(lambdaEncounterd, makeCharKey(buf)) == NULL) {
                insert(lambdaEncounterd, makeCharKey(buf), NULL);
                //CREATE THE LAMBDA IN GLOBAL SCOPE
                Instructions *beginGlobalBLock = newInstruction();
                beginGlobalBLock->kind = METADATA_BEGIN_GLOBAL_BLOCK;
                beginGlobalBLock->val.functionCall.function = buf;
                appendInstructions(beginGlobalBLock);

                Instructions *label = newInstruction();
                label->val.functionHead.label = buf;
                label->val.functionHead.distance = symbolTable->distanceFromRoot + 1;
                label->val.functionHead.temporary = currentTemporary;
                label->val.functionHead.pointerSet = getPointerCountForBody(
                        term->val.lambdaD.lambda->body->declarationList);
                label->kind = INSTRUCTION_FUNCTION_LABEL;
                currentTemporary++;

                SymbolTable *st = NULL;
                if (term->val.lambdaD.lambda->body->declarationList != NULL) {
                    st = term->val.lambdaD.lambda->body->declarationList->declaration->symbolTable;
                } else if (term->val.lambdaD.lambda->body->statementList != NULL) {
                    st = term->val.lambdaD.lambda->body->statementList->statement->symbolTable;
                }
                label->val.functionHead.tableForFunction = st;

                //Readjust arg
                appendInstructions(label);

                //For args we generate metadata for later (maybe this is useless, who knows?)
                VarDelList *declarationList = term->val.lambdaD.lambda->declarationList;
                size_t max = 1;

                size_t regForMoving = currentTemporary;
                currentTemporary++;

                //We need to move from opposite direction since pushing on stack goes opposite
                while (declarationList != NULL) {
                    max++;
                    declarationList = declarationList->next;
                }

                size_t counter = 0;

                //And the lambda context
                Instructions *arg = newInstruction();
                arg->kind = METADATA_FUNCTION_ARGUMENT;
                arg->val.args.argNum = counter;
                arg->val.args.moveReg = regForMoving;
                arg->val.args.stackNum = max - counter - 1;
                appendInstructions(arg);
                counter++;

                declarationList = term->val.lambdaD.lambda->declarationList;
                while (declarationList != NULL) {

                    //Create arg instr for this argument
                    Instructions *arg = newInstruction();
                    arg->kind = METADATA_FUNCTION_ARGUMENT;
                    arg->val.args.argNum = counter;
                    arg->val.args.moveReg = regForMoving;
                    arg->val.args.stackNum = max - counter - 1;
                    appendInstructions(arg);

                    declarationList = declarationList->next;
                    counter++;
                }

                lambdaArgCount = (int) counter;
                generateInstructionTree(term->val.lambdaD.lambda->body);

                Instructions *endGlobalBLock = newInstruction();
                endGlobalBLock->kind = METADATA_END_GLOBAL_BLOCK;
                appendInstructions(endGlobalBLock);
            }

            inLambdaContext = false;

            Instructions *cpy = newInstruction();
            cpy->kind = INSTRUCTION_COPY;
            cpy->val.arithmetic2.source = arrayTemp;
            cpy->val.arithmetic2.dest = currentTemporary;
            appendInstructions(cpy);
            currentTemporary++;
            return currentTemporary - 1;
        } break;
        case classDowncastk: {
            //We simply want to return the original class + an offset which is the downcasted version
            //We find the offset by finding the first declaration in the downcast class and comapring them

            SYMBOL *downcastSym = getSymbol(symbolTable, term->val.classDowncastD.toCastTo->val.typeClass.classId);
            SYMBOL *varTypeSym = getSymbol(symbolTable,
                    unwrapVariable(term->val.classDowncastD.var, symbolTable)->val.typeClass.classId);

            //Must be class
            SymbolTable *bodyTable = varTypeSym->value->val.typeClassD.tableForClassBody;
            Declaration *toSearchFor = downcastSym->value->val.typeClassD.declarationList->declaration;

            char *identifier = NULL;

            if (toSearchFor->kind == declValK) {
                identifier = toSearchFor->val.valD.id;
            } else if (toSearchFor->kind == declVarK) {
                identifier = toSearchFor->val.varD.id;
            } else if (toSearchFor->kind == declVarsK) {
                identifier = toSearchFor->val.varsD.var->val.varD.id;
            }

            SYMBOL *offsetSymbol = getSymbol(bodyTable, identifier);

            size_t variableTemp = generateInstructionsForVariableAccess(term->val.classDowncastD.var, symbolTable);

            Instructions *check = newInstruction();
            check->kind = RUNTIME_NULLPTR_CHECK;
            check->val.divZeroTemp = variableTemp;
            appendInstructions(check);

            size_t downcastOffset = offsetSymbol->uniqueIdForScope;

            Instructions *constOffset = newInstruction();
            constOffset->kind = INSTRUCTION_CONST;
            constOffset->val.constant.value = (int)downcastOffset * POINTER_SIZE;
            constOffset->val.constant.temp= currentTemporary;
            size_t constOffsetTemp = currentTemporary;
            appendInstructions(constOffset);
            currentTemporary++;

            Instructions *instruction = newInstruction();
            instruction->kind = INSTRUCTION_ADD;
            instruction->val.arithmetic2.source = constOffsetTemp;
            instruction->val.arithmetic2.dest = variableTemp;
            appendInstructions(instruction);

            return variableTemp;
        } break;
        case shorthandCallK: {
            //Give arguments on stack
            //For each expression argument, evaluate it and push it to the stack
            size_t fncPtrTemp = generateInstructionsForVariableAccess(term->val.shorthandCallD.var, symbolTable);

            Instructions *check = newInstruction();
            check->kind = RUNTIME_NULLPTR_CHECK;
            check->val.divZeroTemp = fncPtrTemp;
            appendInstructions(check);
/*
            //We need to save the current static link pointer
            Instructions *push = newInstruction();
            push->kind = COMPLEX_SAVE_STATIC_LINK;
            push->val.pushPopStaticLink.staticLinkDepth = (size_t)staticLinkDepth;
            push->val.pushPopStaticLink.temporary = currentTemporary;
            appendInstructions(push);
            currentTemporary++;*/

            Instructions *saveall = newInstruction();
            saveall->kind = COMPLEX_SAVE_ALL;
            //appendInstructions(saveall);

            ExpressionList *expressionList = term->val.shorthandCallD.expressionList;

            int toAdd = 0;

            while (expressionList != NULL) {
                //Evaluate
                size_t temporaryForArgument = generateInstructionsForExpression(expressionList->expression, symbolTable);

                //Then push
                Instructions *push = newInstruction();
                push->kind = INSTRUCTION_PUSH;
                push->val.tempToPush = temporaryForArgument;
                appendInstructions(push);

                toAdd++;
                expressionList = expressionList->next;
            }

            Instructions *captureOffset = newInstruction();
            captureOffset->kind = INSTRUCTION_CONST;
            captureOffset->val.constant.value = POINTER_SIZE;
            captureOffset->val.constant.temp = currentTemporary;
            size_t captureTemp = currentTemporary;
            appendInstructions(captureOffset);
            currentTemporary++;

            //Deref an extra time, lambda's are always double dereferenced
            Instructions *capturePtr = newInstruction();
            capturePtr->kind = COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET;
            capturePtr->val.dereferenceOffset.ptrTemp = fncPtrTemp;
            capturePtr->val.dereferenceOffset.offsetTemp = captureTemp;
            capturePtr->val.dereferenceOffset.size = 1;
            //fncPtrTemp = currentTemporary;
            appendInstructions(capturePtr);
            //currentTemporary++;

            Instructions *debug = newInstruction();
            debug->kind = METADATA_DEBUG_INFO;
            debug->val.debugInfo = "CAPTURE PUSH";
            appendInstructions(debug);

            //Push capture pointer
            Instructions *push2 = newInstruction();
            push2->kind = INSTRUCTION_PUSH;
            push2->val.tempToPush = fncPtrTemp;
            appendInstructions(push2);
            toAdd++;

            fncPtrTemp = generateInstructionsForVariableAccess(term->val.shorthandCallD.var, symbolTable);

            //Off by one since we have arr of ptrs and first is len
            Instructions *num = newInstruction();
            num->kind = INSTRUCTION_CONST;
            num->val.constant.value = 0;
            num->val.constant.temp = currentTemporary;
            size_t numTemp = currentTemporary;
            appendInstructions(num);
            currentTemporary++;

            //Deref an extra time, lambda's are always double dereferenced
            Instructions *ptrAccess = newInstruction();
            ptrAccess->kind = COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET;
            ptrAccess->val.dereferenceOffset.ptrTemp = fncPtrTemp;
            ptrAccess->val.dereferenceOffset.offsetTemp = numTemp;
            ptrAccess->val.dereferenceOffset.size = 1;
            //fncPtrTemp = currentTemporary;
            appendInstructions(ptrAccess);
            //currentTemporary++;

            //Then call
            Instructions *call = newInstruction();
            call->kind = INSTRUCTION_REGISTER_CALL;
            call->val.callRegister = fncPtrTemp;
            appendInstructions(call);

            Instructions *sp = newInstruction();
            sp->kind = INSTRUCTION_ADD_STACK_PTR;
            sp->val.toAddStackPtr = toAdd * POINTER_SIZE;
            appendInstructions(sp);

            Instructions *restoreall = newInstruction();
            restoreall->kind = COMPLEX_RESTORE_ALL;
            //appendInstructions(restoreall);

            call->val.functionCall.restore = restoreall;
            call->val.functionCall.save = saveall;

            //We need to restore the static link
            Instructions *popLink = newInstruction();
            popLink->kind = COMPLEX_RESTORE_STATIC_LINK;
            popLink->val.pushPopStaticLink.staticLinkDepth = symbolTable->distanceFromRoot;
            popLink->val.pushPopStaticLink.temporary = currentTemporary;
            appendInstructions(popLink);
            currentTemporary++;

            Instructions *cpy = newInstruction();
            cpy->kind = INSTRUCTION_COPY;
            cpy->val.arithmetic2.source = 0;
            cpy->val.arithmetic2.dest = currentTemporary;
            appendInstructions(cpy);
            currentTemporary++;

            return currentTemporary - 1;
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
                    instruction->val.arithmetic2.source = rhsTemp;
                    instruction->val.arithmetic2.dest = lhsTemp;
                    appendInstructions(instruction);
                    toReturn = lhsTemp;
                } break;
                case opDivK: {
                    Instructions *check = newInstruction();
                    check->kind = RUNTIME_DIV_ZERO;
                    check->val.divZeroTemp = rhsTemp;
                    appendInstructions(check);

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
                    instruction->val.arithmetic2.source = rhsTemp;
                    instruction->val.arithmetic2.dest = lhsTemp;
                    appendInstructions(instruction);
                    toReturn = lhsTemp;
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
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);

                    Instructions *abs = newInstruction();
                    abs->kind = COMPLEX_ABS_VALUE;
                    abs->val.arithmetic2.source = rhsTemp;
                    abs->val.arithmetic2.dest = currentTemporary++;
                    appendInstructions(abs);

                    Instructions *constrain = newInstruction();
                    constrain->kind = COMPLEX_CONSTRAIN_BOOLEAN;
                    constrain->val.tempToConstrain = currentTemporary - 1;
                    appendInstructions(constrain);

                    toReturn = currentTemporary - 1;
                } break;
                case opEqualityK: {
                    Instructions *instruction = newInstruction();
                    instruction->kind = INSTRUCTION_MINUS;
                    instruction->val.arithmetic2.source = lhsTemp;
                    instruction->val.arithmetic2.dest = rhsTemp;
                    appendInstructions(instruction);

                    Instructions *abs = newInstruction();
                    abs->kind = COMPLEX_ABS_VALUE;
                    abs->val.arithmetic2.source = rhsTemp;
                    abs->val.arithmetic2.dest = currentTemporary++;
                    size_t absReg = currentTemporary - 1;
                    appendInstructions(abs);

                    Instructions *constrain = newInstruction();
                    constrain->kind = COMPLEX_CONSTRAIN_BOOLEAN;
                    constrain->val.tempToConstrain = absReg;
                    appendInstructions(constrain);

                    Instructions *contOne = newInstruction();
                    contOne->kind = INSTRUCTION_CONST;
                    contOne->val.constant.value = 1;
                    contOne->val.constant.temp = currentTemporary++;
                    size_t constOneTemp = currentTemporary - 1;
                    appendInstructions(contOne);

                    Instructions *invert = newInstruction();
                    invert->kind = INSTRUCTION_MINUS;
                    invert->val.arithmetic2.source = absReg;
                    invert->val.arithmetic2.dest = constOneTemp;
                    appendInstructions(invert);

                    toReturn = constOneTemp;
                } break;
                case opGreaterK: {
                    //TODO Use smart x86 instruction for this
                    //x > y -> lhs > rhs
                    //subtract x from y and check if the result is > 0 later
                    //also constrain to boolean
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
                case opLessK: {
                    //TODO Use smart x86 instruction for this
                    //Same as above, but lhs and rhs flipped
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
                case opGeqK: {
                    //TODO Use smart x86 instruction for this
                    //Can be hacked by using greater but adding a constant of one
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
                case opLeqK: {
                    //TODO Use smart x86 instruction for this
                    //Same as above, but lhs and rhs flipped
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

    ret->dataFlowEntry = NULL;
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
        case writeNL: {
            Instructions *instructions = newInstruction();
            instructions->kind = INSTRUCTION_WRITE_NL;
            appendInstructions(instructions);
        } break;
        case writeAny:
        case statWriteK: {
            if (evaluateExpressionType(statement->val.writeD.exp, statement->symbolTable)->kind == typeCharK) {
                Instructions *instructions = newInstruction();
                instructions->kind = INSTRUCTION_WRITE_CHAR;
                instructions->val.tempToWrite = generateInstructionsForExpression(statement->val.writeD.exp, statement->symbolTable);
                appendInstructions(instructions);
            } else {
                Instructions *instructions = newInstruction();
                instructions->kind = INSTRUCTION_WRITE;
                instructions->val.tempToWrite = generateInstructionsForExpression(statement->val.writeD.exp, statement->symbolTable);
                appendInstructions(instructions);
            }
        } break;
        case statAllocateK: {
            Type *tpe = unwrapTypedef(unwrapVariable(statement->val.allocateD.var, statement->symbolTable), statement->symbolTable, NULL, true);

            size_t fieldCount = 0;

            SYMBOL *baseSym = getSymbolForBaseVariable(statement->val.allocateD.var, statement->symbolTable);

            Instructions *allocdebug = newInstruction();
            allocdebug->kind = METADATA_DEBUG_INFO;
            allocdebug->val.debugInfo = "ALLOC";
            appendInstructions(allocdebug);

            Instructions *debug = newInstruction();
            debug->kind = METADATA_DEBUG_INFO;
            debug->val.debugInfo = variableToString(statement->val.allocateD.var);
            appendInstructions(debug);

            SortedSet *bodySet;

            if (tpe->kind == typeRecordK) {
                //Record
                VarDelList *iter = tpe->val.recordType.types;

                bodySet = initHeadedSortedSet();
                while (iter != NULL) {
                    Type *unwrapped = unwrapTypedef(iter->type, statement->symbolTable, NULL, true);
                    if (unwrapped->kind != typeIntK && unwrapped->kind != typeBoolK && unwrapped->kind != typeCharK) {
                        insertSortedSet(bodySet, (int)fieldCount);
                    }

                    fieldCount++;
                    iter = iter->next;
                }
            } else {
                //Class
                SYMBOL *classSymbol = getSymbol(statement->symbolTable, tpe->val.typeClass.classId);

                DeclarationList *iter = classSymbol->value->val.typeClassD.declarationList;
                bodySet = getPointerCountForBody(classSymbol->value->val.typeClassD.declarationList);

                while (iter != NULL) {
                    fieldCount++;
                    iter = iter->next;
                }
            }

            Instructions *num = newInstruction();
            num->kind = INSTRUCTION_CONST;
            num->val.constant.value = (int)fieldCount;
            num->val.constant.temp = currentTemporary;
            appendInstructions(num);
            size_t constNum = currentTemporary;
            currentTemporary++;

            Instructions *ret = newInstruction();
            ret->kind = COMPLEX_ALLOCATE;
            ret->val.allocate.allocationType = ALLOC_RECORD_CLASS;
            ret->val.allocate.pointerSet = bodySet;
            ret->val.allocate.timesTemp = constNum;
            ret->val.allocate.intermediate = currentTemporary++;
            ret->val.allocate.eleSize = POINTER_SIZE;
            size_t allocPtrTemp = 0;
            appendInstructions(ret);

            //Save class ptr
            Instructions *push = newInstruction();
            push->kind = INSTRUCTION_PUSH;
            push->val.tempToPush = allocPtrTemp;
            appendInstructions(push);

            //Instructions for getting getting the address we need to move the pointer to
            generateInstructionsForVariableSave(statement->val.allocateD.var, statement->symbolTable, allocPtrTemp, false);

            //If there are any vals in the class "construct" them
            if (tpe->kind == typeClassK) {
                inClassContextCurrent = true;
                className = tpe->val.typeClass.classId;

                SYMBOL *classSymbol = getSymbol(statement->symbolTable, tpe->val.typeClass.classId);

                classGenericCount = 0;
                TypeList *generics = classSymbol->value->val.typeClassD.generics;
                while (generics != NULL) {
                    classGenericCount++;
                    generics = generics->next;
                }

                DeclarationList *iter = classSymbol->value->val.typeClassD.declarationList;

                while (iter != NULL) {
                    if (iter->declaration->kind == declValK) {
                        if (evaluateExpressionType(iter->declaration->val.valD.rhs, iter->declaration->symbolTable)->kind == typeLambdaK) {
                            size_t exprResult = generateInstructionsForExpression(iter->declaration->val.valD.rhs, iter->declaration->symbolTable);

                            Instructions *constPtr = newInstruction();
                            constPtr->kind = INSTRUCTION_CONST;
                            constPtr->val.constant.value = POINTER_SIZE;
                            constPtr->val.constant.temp = currentTemporary;
                            size_t ptrSizeTmp = currentTemporary;
                            appendInstructions(constPtr);
                            currentTemporary++;

                            Instructions *pop = newInstruction();
                            pop->kind = INSTRUCTION_POP;
                            pop->val.tempToPopInto = allocPtrTemp;
                            appendInstructions(pop);

                            Instructions *ptrAccess = newInstruction();
                            ptrAccess->kind = INSTRUCTION_MOVE_TO_OFFSET;
                            ptrAccess->val.moveToOffset.ptrTemp = exprResult;
                            ptrAccess->val.moveToOffset.offsetTemp = ptrSizeTmp;
                            ptrAccess->val.moveToOffset.tempToMove = allocPtrTemp;
                            appendInstructions(ptrAccess);

                            Instructions *push = newInstruction();
                            push->kind = INSTRUCTION_PUSH;
                            push->val.tempToPush = allocPtrTemp;
                            appendInstructions(push);

                            Variable *valAccess = NEW(Variable);
                            valAccess->kind = recordLookupK;
                            valAccess->val.recordLookupD.id = iter->declaration->val.valD.id;
                            valAccess->val.recordLookupD.var = statement->val.allocateD.var;

                            generateInstructionsForVariableSave(valAccess, statement->symbolTable, exprResult, false);
                        } else {
                            size_t exprResult = generateInstructionsForExpression(iter->declaration->val.valD.rhs, iter->declaration->symbolTable);

                            Variable *valAccess = NEW(Variable);
                            valAccess->kind = recordLookupK;
                            valAccess->val.recordLookupD.id = iter->declaration->val.valD.id;
                            valAccess->val.recordLookupD.var = statement->val.allocateD.var;

                            generateInstructionsForVariableSave(valAccess, statement->symbolTable, exprResult, false);
                        }
                    }

                    iter = iter->next;
                }

                inClassContextCurrent = false;
            }

            if (tpe->kind == typeClassK) {
                //We may be able to construct
                if (statement->val.allocateD.constructorList != NULL) {
                    //Create constructor name
                    SYMBOL *classSymbol = getSymbol(statement->symbolTable, tpe->val.typeClass.classId);
                    char *baseName = tpe->val.typeClass.classId;
                    int extra = (int)strlen("_constructor") + 1;
                    char *buf = (char*)malloc(sizeof(char) * (strlen(baseName) + extra));

                    sprintf(buf, "%s_constructor", baseName);

                    Instructions *pop = newInstruction();
                    pop->kind = INSTRUCTION_POP;
                    pop->val.tempToPopInto = allocPtrTemp;
                    appendInstructions(pop);

                    Instructions *push = newInstruction();
                    push->kind = INSTRUCTION_PUSH;
                    push->val.tempToPush = allocPtrTemp;
                    appendInstructions(push);

                    //push args first, then class context ptr
                    ExpressionList *argIter = statement->val.allocateD.constructorList;

                    int toAdd = 0;

                    while (argIter != NULL) {

                        size_t tempForArg = generateInstructionsForExpression(argIter->expression, statement->symbolTable);

                        //Push arg
                        Instructions *push = newInstruction();
                        push->kind = INSTRUCTION_PUSH;
                        push->val.tempToPush = tempForArg;
                        appendInstructions(push);

                        toAdd++;
                        argIter = argIter->next;
                    }


                    toAdd++;
                    //Push class ptr
                    Instructions *clsPush = newInstruction();
                    clsPush->kind = INSTRUCTION_PUSH;
                    clsPush->val.tempToPush = allocPtrTemp;
                    appendInstructions(clsPush);

                    Instructions *call = newInstruction();
                    call->kind = INSTRUCTION_FUNCTION_CALL;
                    call->val.functionCall.function = buf;
                    appendInstructions(call);

                    Instructions *sp = newInstruction();
                    sp->kind = INSTRUCTION_ADD_STACK_PTR;
                    sp->val.toAddStackPtr = toAdd * POINTER_SIZE;
                    appendInstructions(sp);

                }
            }

            Instructions *pop = newInstruction();
            pop->kind = INSTRUCTION_POP;
            pop->val.tempToPopInto = allocPtrTemp;
            appendInstructions(pop);

            //Instructions *endAlloc = newInstruction();
            //endAlloc->kind = COMPLEX_ALLOCATE_END;
            //appendInstructions(endAlloc);
        } break;
        case statAllocateLenK: {
            size_t lenExp = generateInstructionsForExpression(statement->val.allocateLenD.len, statement->symbolTable);

            Instructions *check = newInstruction();
            check->kind = RUNTIME_NEGATIVE_ALLOC;
            check->val.divZeroTemp = lenExp;
            appendInstructions(check);

            SYMBOL *symbol = getSymbolForBaseVariable(statement->val.allocateD.var, statement->symbolTable);

            Type *type = unwrapTypedef(unwrapVariable(statement->val.allocateLenD.var, statement->symbolTable), statement->symbolTable, NULL, true);
            Instructions *ret = newInstruction();
            ret->kind = COMPLEX_ALLOCATE;
            ret->val.allocate.timesTemp = lenExp;
            ret->val.allocate.eleSize = POINTER_SIZE;
            ret->val.allocate.intermediate = currentTemporary++;
            if (type->val.arrayType.type->kind == typeIntK || type->val.arrayType.type->kind == typeBoolK || type->val.arrayType.type->kind == typeCharK) {
                ret->val.allocate.allocationType = ALLOC_ARR_OF_PRIM;
            } else {
                ret->val.allocate.allocationType = ALLOC_ARR_OF_PTR;
            }
            size_t allocPtrTemp = 0;
            appendInstructions(ret);
            //currentTemporary++;

            //Instructions for getting getting the address we need to move the pointer to
            generateInstructionsForVariableSave(statement->val.allocateLenD.var, statement->symbolTable, allocPtrTemp, false);

        } break;
        case statIfK: {
            //TODO

            size_t boolTemp = generateInstructionsForExpression(statement->val.ifD.exp, statement->symbolTable);
            Instructions *constant = newInstruction();
            constant->kind = INSTRUCTION_CONST;
            constant->val.constant.temp = currentTemporary;
            constant->val.constant.value = 1;
            appendInstructions(constant);
            size_t constantTemp = currentTemporary;
            currentTemporary++;
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

            size_t boolTemp = generateInstructionsForExpression(statement->val.ifElD.exp, statement->symbolTable);
            Instructions *constant = newInstruction();
            constant->kind = INSTRUCTION_CONST;
            constant->val.constant.temp = currentTemporary;
            constant->val.constant.value = 1;
            appendInstructions(constant);
            size_t constantTemp = currentTemporary;
            currentTemporary++;
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


            size_t boolTemp = generateInstructionsForExpression(statement->val.ifElD.exp, statement->symbolTable);
            Instructions *constant = newInstruction();
            constant->kind = INSTRUCTION_CONST;
            constant->val.constant.temp = currentTemporary;
            constant->val.constant.value = 1;
            appendInstructions(constant);
            size_t constantTemp = currentTemporary;
            currentTemporary++;
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

            generateInstructionsForVariableSave(statement->val.assignmentD.var, statement->symbolTable, expressionTemp, false);

            if (unwrapVariable(statement->val.assignmentD.var, statement->symbolTable)->kind == typeLambdaK) {
                if (unwrapTypedef(
                        symbol->value->val.typeD.tpe,
                        statement->symbolTable, NULL, true)->kind == typeClassK) {
                    //We need to bind class to second slot offset POINTER_SIZE
                    //expressionTemp holds our 2 arr

                    //Grab the class into temp
                    size_t classTemp = generateInstructionsForVariableAccess(statement->val.assignmentD.var, statement->symbolTable);

                    Instructions *constPtr = newInstruction();
                    constPtr->kind = INSTRUCTION_CONST;
                    constPtr->val.constant.value = POINTER_SIZE;
                    constPtr->val.constant.temp = currentTemporary;
                    size_t ptrSizeTmp = currentTemporary;
                    appendInstructions(constPtr);
                    currentTemporary++;

                    Instructions *ptrAccess = newInstruction();
                    ptrAccess->kind = INSTRUCTION_MOVE_TO_OFFSET;
                    ptrAccess->val.moveToOffset.ptrTemp = expressionTemp;
                    ptrAccess->val.moveToOffset.offsetTemp = ptrSizeTmp;
                    ptrAccess->val.moveToOffset.tempToMove = classTemp;
                    appendInstructions(ptrAccess);
                }
            }
        } break;
        case emptyK: {
            generateInstructionsForExpression(statement->val.empty.exp, statement->symbolTable);
        } break;
        case gcK: {
            Instructions *gc = newInstruction();
            gc->kind = COMPLEX_GARBAGE_COLLECT;
            appendInstructions(gc);
        } break ;
        case gcDebugK : {
            Instructions *gcdebug = newInstruction();
            gcdebug->kind = INSTRUCTION_FUNCTION_CALL;
            gcdebug->val.functionCall.function = "gcPrintDebug";
            appendInstructions(gcdebug);
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
            //Append depdth to name
            char *baseName = declaration->val.functionD.function->head->indentifier;
            int extra = 16;
            char *buf = (char*)malloc(sizeof(char) * (strlen(baseName) + extra));

            sprintf(buf, "%s__%i", baseName, (int)symbol->distanceFromRoot);

            Instructions *beginGlobalBLock = newInstruction();
            beginGlobalBLock->kind = METADATA_BEGIN_GLOBAL_BLOCK;
            beginGlobalBLock->val.functionCall.function = buf;
            appendInstructions(beginGlobalBLock);



            Instructions *label = newInstruction();
            label->val.functionHead.label = buf;
            label->val.functionHead.distance = symbol->distanceFromRoot + 1;
            label->val.functionHead.temporary = currentTemporary;
            label->val.functionHead.pointerSet =
                    getPointerCountForBody(declaration->val.functionD.function->body->declarationList);
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
            size_t max = 0;

            size_t  regForMoving = currentTemporary;
            currentTemporary++;

            //We need to move from opposite direction since pushing on stack goes opposite
            while (declarationList != NULL) {
                max++;
                declarationList = declarationList->next;
            }

            size_t counter = 0;
            declarationList = declaration->val.functionD.function->head->declarationList;
            while (declarationList != NULL) {

                //Create arg instr for this argument
                Instructions *arg = newInstruction();
                arg->kind = METADATA_FUNCTION_ARGUMENT;
                arg->val.args.argNum = counter;
                arg->val.args.moveReg = regForMoving;
                arg->val.args.stackNum = max - counter - 1;
                appendInstructions(arg);

                declarationList = declarationList->next;
                counter++;
            }

            generateInstructionTree(declaration->val.functionD.function->body);

            Instructions *endGlobalBLock = newInstruction();
            endGlobalBLock->kind = METADATA_END_GLOBAL_BLOCK;
            appendInstructions(endGlobalBLock);
        } break;
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

            generateInstructionsForVariableSave(tmpVar, declaration->symbolTable, expressionTemp, false);
        } break;
        case declClassK: {
            inClassContextCurrent = true;
            Constructor *constructor = declaration->val.classD.constructor;

            if (constructor != NULL ) {
                currentlyInConstructorContext = true;
                lambdaDefineScope = (int)declaration->symbolTable->distanceFromRoot + 1;
                char *baseName = declaration->val.classD.id;
                int extra = (int)strlen("_constructor") + 1;
                char *buf = (char*)malloc(sizeof(char) * (strlen(baseName) + extra));

                sprintf(buf, "%s_constructor", baseName);

                //Create constructor
                Instructions *beginGlobalBLock = newInstruction();
                beginGlobalBLock->kind = METADATA_BEGIN_GLOBAL_BLOCK;
                beginGlobalBLock->val.functionCall.function = buf;
                appendInstructions(beginGlobalBLock);

                Instructions *label = newInstruction();
                label->val.functionHead.label = buf;
                label->val.functionHead.distance = declaration->symbolTable->distanceFromRoot + 1;
                label->val.functionHead.temporary = currentTemporary;
                label->val.functionHead.pointerSet = getPointerCountForBody(
                        constructor->body->declarationList);
                label->kind = INSTRUCTION_FUNCTION_LABEL;
                currentTemporary++;

                SymbolTable *st = NULL;
                if (constructor->body->declarationList != NULL) {
                    st = constructor->body->declarationList->declaration->symbolTable;
                } else if (constructor->body->statementList != NULL) {
                    st = constructor->body->statementList->statement->symbolTable;
                }
                label->val.functionHead.tableForFunction = st;

                //Readjust arg
                appendInstructions(label);

                //For args we generate metadata for later (maybe this is useless, who knows?)
                VarDelList *declarationList = constructor->declarationList;
                size_t max = 1;

                size_t regForMoving = currentTemporary;
                currentTemporary++;

                //We need to move from opposite direction since pushing on stack goes opposite
                while (declarationList != NULL) {
                    max++;
                    declarationList = declarationList->next;
                }

                size_t counter = 0;

                //And the lambda context
                Instructions *arg = newInstruction();
                arg->kind = METADATA_FUNCTION_ARGUMENT;
                arg->val.args.argNum = counter;
                arg->val.args.moveReg = regForMoving;
                arg->val.args.stackNum = max - counter - 1;
                appendInstructions(arg);
                counter++;

                declarationList = constructor->declarationList;
                while (declarationList != NULL) {

                    //Create arg instr for this argument
                    Instructions *arg = newInstruction();
                    arg->kind = METADATA_FUNCTION_ARGUMENT;
                    arg->val.args.argNum = counter;
                    arg->val.args.moveReg = regForMoving;
                    arg->val.args.stackNum = max - counter - 1;
                    appendInstructions(arg);

                    declarationList = declarationList->next;
                    counter++;
                }

                lambdaArgCount = (int) counter;
                generateInstructionTree(constructor->body);

                Instructions *endGlobalBLock = newInstruction();
                endGlobalBLock->kind = METADATA_END_GLOBAL_BLOCK;
                appendInstructions(endGlobalBLock);
                currentlyInConstructorContext = false;
            }
            inClassContextCurrent = false;
        } break;
    }
}

void insertForType(SortedSet *sortedSet, SYMBOL *symbol, SymbolTable *symbolTable) {
    if (symbol->value->kind == symTypeClassK) {
        return;
    }

    if (symbol->value->kind == typeFunctionK && !symbol->value->val.typeFunctionD.isLambda) {
        return;
    }

    Type* typeToUnwrap = symbol->value->val.typeD.tpe;
    if (symbol->value->kind == typeFunctionK && symbol->value->val.typeFunctionD.isLambda) {
        insertSortedSet(sortedSet, (int)symbol->uniqueIdForScope);
        return;
    }

    Type *unwrapped = unwrapTypedef(typeToUnwrap, symbolTable, NULL, true);

    if (unwrapped == NULL) {
        //We handle this later
        return;
    }

    if (unwrapped->kind != typeIntK && unwrapped->kind != typeBoolK && unwrapped->kind != typeCharK) {
        insertSortedSet(sortedSet, (int)symbol->uniqueIdForScope);
    }
}

void getPointerCountForDecl(SortedSet *sortedSet, Declaration *declaration) {
    if (declaration == NULL) {
        return;
    }

    switch (declaration->kind) {
        case declVarK: {
            insertForType(sortedSet, getSymbol(declaration->symbolTable, declaration->val.varD.id), declaration->symbolTable);
        } break;
        case declVarsK: {
            getPointerCountForDecl(sortedSet, declaration->val.varsD.var);

            if (declaration->val.varsD.next == NULL) {
                return;
            } else {
                getPointerCountForDecl(sortedSet, declaration->val.varsD.next);
            }
        } break;
        case declValK: {
            insertForType(sortedSet, getSymbol(declaration->symbolTable, declaration->val.valD.id), declaration->symbolTable);
        } break;
        default:
            break;
    }
}

SortedSet *getPointerCountForBody(DeclarationList *declarationList) {
    DeclarationList *iter = declarationList;
    SortedSet *sortedSet = initHeadedSortedSet();

    while (iter != NULL) {
        getPointerCountForDecl(sortedSet, iter->declaration);

        iter = iter->next;
    }

    return sortedSet;
}

Instructions* generateInstructionTree(Body *body) {
    staticLinkDepth++;
    //Save temporary counter
    bool createMain = false;

    if (!mainCreated) {
        createMain = true;
        mainCreated = true;
    }

    if (lambdaEncounterd == NULL) {
        lambdaEncounterd = initMap(20);
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


    while (declarationList != NULL) {
        if (declarationList->declaration->kind != declValK) {
            generateInstructionTreeForDeclaration(declarationList->declaration);
        }
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
        declsEnd->val.mainHeader.tableForFunction = st;
        declsEnd->val.mainHeader.pointerSet = getPointerCountForBody(body->declarationList);

        appendInstructions(declsEnd);
    }

    declarationList = body->declarationList;
    while (declarationList != NULL) {
        if (declarationList->declaration->kind == declValK) {
            generateInstructionTreeForDeclaration(declarationList->declaration);
        }

        declarationList = declarationList->next;
    }

    while (statementList != NULL) {
        generateInstructionTreeForStatement(statementList->statement);
        statementList = statementList->next;
    }

    if (!createMain) {
        currentInstruction->next = newInstruction();
        currentInstruction = currentInstruction->next;
        currentInstruction->kind = METADATA_END_BODY_BLOCK;
    }

    if (currentTemporary > maxTemporary) {
        maxTemporary = currentTemporary;
    }
    currentTemporary = restoreTemporary;

    staticLinkDepth--;
    return instructionHead;
}