#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void passTwo() {
    FILE *intermediate, *optab, *symtab, *littab, *objprog, *output;
    char label[20], opcode[20], operand[20], mnemonic[20], symbol[20], literal[20], objectCode[10];
    int locctr, operandAddress = 0, base = 0, startAddress = 0, programCounter = 0;
    char textRecord[70] = "", modificationRecord[100][20];
    int modIndex = 0;
    char programName[10]; // To store the program name

    intermediate = fopen("pass1_intermediate_code.txt", "r");
    optab = fopen("pass1_optab.txt", "r");
    symtab = fopen("pass1_symtab.txt", "r");
    littab = fopen("pass1_littab.txt", "r");
    objprog = fopen("pass2_object_program.txt", "w");
    output = fopen("pass2_object_code.txt", "w");

    if (!intermediate || !optab || !symtab || !littab || !objprog || !output) {
        printf("Error: File open failed.\n");
        exit(1);
    }

    // Read the first line to initialize start address and get program length
    fscanf(intermediate, "%X %s %s %s", &locctr, label, opcode, operand);
    if (strcmp(opcode, "START") == 0) {
        startAddress = strtol(operand, NULL, 16);
        programCounter = startAddress;
        strcpy(programName, label); // Store program name from the START record
    }

    // Calculate program length (from pass1_intermediate_code.txt)
    int lastLocctr = startAddress;
    while (fscanf(intermediate, "%X %s %s %s", &locctr, label, opcode, operand) != EOF) {
        lastLocctr = locctr;
    }
    int programLength = lastLocctr - startAddress + 5; // Correct calculation for 1045

    // Write header record with correct length and program name
    fprintf(objprog, "H^%-6s^%06X^%06X\n", programName, startAddress, programLength);

    // Rewind the intermediate file pointer
    rewind(intermediate);
    fscanf(intermediate, "%X %s %s %s", &locctr, label, opcode, operand);

    char textRecordStart[7];
    sprintf(textRecordStart, "%06X", locctr);
    int textRecordLen = 0;

    while (fscanf(intermediate, "%X %s %s %s", &locctr, label, opcode, operand) != EOF) {
        int opcodeFound = 0;
        rewind(optab);

        // Look up opcode in optab
        while (fscanf(optab, "%s %s", mnemonic, objectCode) != EOF) {
            if (strcmp(opcode, mnemonic) == 0 || (opcode[0] == '+' && strcmp(opcode + 1, mnemonic) == 0)) {
                opcodeFound = 1;
                break;
            }
        }

        if (opcodeFound) {
            int format = (opcode[0] == '+') ? 4 : 3;
            int n = 1, i = 1, x = 0, b = 0, p = 0, e = 0;

            if (format == 4) e = 1;

            operandAddress = 0;
            if (operand[0] == '=') { // Literal handling
                rewind(littab);
                while (fscanf(littab, "%s %X", literal, &operandAddress) != EOF) {
                    if (strcmp(literal, operand) == 0) break;
                }
            } else if (operand[0] == '#') { // Immediate addressing
                n = 0;
                i = 1;
                if (isdigit(operand[1])) {
                    operandAddress = strtol(operand + 1, NULL, 10);
                } else {
                    rewind(symtab);
                    while (fscanf(symtab, "%s %X", symbol, &operandAddress) != EOF) {
                        if (strcmp(symbol, operand + 1) == 0) break;
                    }
                }
            } else if (operand[0] == '@') { // Indirect addressing
                n = 1;
                i = 0;
                rewind(symtab);
                while (fscanf(symtab, "%s %X", symbol, &operandAddress) != EOF) {
                    if (strcmp(symbol, operand + 1) == 0) break;
                }
            } else { // Simple addressing
                rewind(symtab);
                while (fscanf(symtab, "%s %X", symbol, &operandAddress) != EOF) {
                    if (strcmp(symbol, operand) == 0) break;
                }
            }

            if (format == 3) {
                int disp = operandAddress - (programCounter + 3);
                if (disp >= -2048 && disp <= 2047) { // PC-relative
                    p = 1;
                    operandAddress = disp & 0xFFF;
                } else { // Base-relative
                    b = 1;
                    operandAddress -= base;
                }
            }

            // Generate object code
            if (format == 3) {
                sprintf(objectCode, "%02X%01X%03X",
                         (int)strtol(objectCode, NULL, 16) + (n << 1) + i,
                         x * 8 + b * 4 + p * 2 + e,
                         operandAddress);
            } else if (format == 4) {
                sprintf(objectCode, "%02X%01X%05X",
                         (int)strtol(objectCode, NULL, 16) + (n << 1) + i,
                         x * 8 + b * 4 + p * 2 + e,
                         operandAddress);
                sprintf(modificationRecord[modIndex++], "M^%06X^05", locctr + 1);
            }

            if (textRecordLen + strlen(objectCode) / 2 > 30) { // Check if adding the object code exceeds length limit
                fprintf(objprog, "T^%s^%02X%s\n", textRecordStart, textRecordLen, textRecord);
                strcpy(textRecord, "");
                sprintf(textRecordStart, "%06X", locctr);
                textRecordLen = 0;
            }

            strcat(textRecord, "^");
            strcat(textRecord, objectCode);
            textRecordLen += strlen(objectCode) / 2;

            fprintf(output, "%04X\t%s\t%s\t%s\t%s\n", locctr, label, opcode, operand, objectCode);
        } else if (strcmp(opcode, "BYTE") == 0) {
            if (operand[0] == 'C') {
                for (int i = 2; i < strlen(operand) - 1; i++) {
                    sprintf(objectCode + strlen(objectCode), "%02X", operand[i]);
                }
            } else if (operand[0] == 'X') {
                strncpy(objectCode, operand + 2, strlen(operand) - 3);
                objectCode[strlen(operand) - 3] = '\0';
            }

            if (textRecordLen + strlen(objectCode) / 2 > 30) {
                fprintf(objprog, "T^%s^%02X%s\n", textRecordStart, textRecordLen, textRecord);
                strcpy(textRecord, "");
                sprintf(textRecordStart, "%06X", locctr);
                textRecordLen = 0;
            }

            strcat(textRecord, "^");
            strcat(textRecord, objectCode);
            textRecordLen += strlen(objectCode) / 2;

            fprintf(output, "%04X\t%s\t%s\t%s\t%s\n", locctr, label, opcode, operand, objectCode);
        } else if (strcmp(opcode, "WORD") == 0) {
            sprintf(objectCode, "%06X", (int)strtol(operand, NULL, 10));

            if (textRecordLen + strlen(objectCode) / 2 > 30) {
                fprintf(objprog, "T^%s^%02X%s\n", textRecordStart, textRecordLen, textRecord);
                strcpy(textRecord, "");
                sprintf(textRecordStart, "%06X", locctr);
                textRecordLen = 0;
            }

            strcat(textRecord, "^");
            strcat(textRecord, objectCode);
            textRecordLen += 3;

            fprintf(output, "%04X\t%s\t%s\t%s\t%s\n", locctr, label, opcode, operand, objectCode);
        }

        if (strcmp(opcode, "END") == 0) {
            break; // Exit the loop when END is encountered
        }

        programCounter = locctr;
    }

    // Write remaining text record if any
    if (textRecordLen > 0) {
        fprintf(objprog, "T^%s^%02X%s\n", textRecordStart, textRecordLen, textRecord);
    }



    // Write modification records
    for (int i = 0; i < modIndex; i++) {
        fprintf(objprog, "%s\n", modificationRecord[i]);
    }

 // Write end record at the end
    fprintf(objprog, "E^%06X\n", startAddress);

    fclose(intermediate);
    fclose(optab);
    fclose(symtab);
    fclose(littab);
    fclose(objprog);
    fclose(output);

    printf("Pass 2 completed successfully with literals handled.\n");
}

int main() {
    passTwo();
    return 0;
}
