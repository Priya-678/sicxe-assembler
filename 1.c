#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINE 1000
#define MAX_LITERALS 100

typedef struct {
    char literal[50];  // Increased size for safety
    int address;
    int size;
    int pool_number;   // Track literal pool number
    int processed;     // Flag to track if literal is processed
} Literal;

void processLiteral(Literal *littab, int *literalCount, char *operand, int *current_pool) {
    // Skip if literal already exists
    for (int i = 0; i < *literalCount; i++) {
        if (strcmp(littab[i].literal, operand) == 0 && !littab[i].processed) {
            return;
        }
    }

    // Add new literal
    strcpy(littab[*literalCount].literal, operand);
    littab[*literalCount].address = -1;
    littab[*literalCount].pool_number = *current_pool;
    littab[*literalCount].processed = 0;

    // Calculate size based on literal type
    if (operand[1] == 'C') {
        // For character literals (=C'ABC')
        littab[*literalCount].size = strlen(operand) - 4;  // Subtract =C''
    } else if (operand[1] == 'X') {
        // For hexadecimal literals (=X'F1')
        littab[*literalCount].size = (strlen(operand) - 4) / 2;  // Each byte is 2 hex digits
    }

    (*literalCount)++;
}

int assignLiteralAddresses(Literal *littab, int literalCount, int locctr, int pool_number) {
    for (int i = 0; i < literalCount; i++) {
        if (littab[i].address == -1 && littab[i].pool_number == pool_number && !littab[i].processed) {
            littab[i].address = locctr;
            littab[i].processed = 1;
            locctr += littab[i].size;
        }
    }
    return locctr;
}

void passOne() {
    char label[50], opcode[50], operand[50], mnemonic[50];
    int start, locctr, literalCount = 0, current_pool = 0;
    char line[MAX_LINE];
    Literal littab[MAX_LITERALS];
    FILE *input, *optab, *symtab, *intermediate, *littabFile;

    input = fopen("pass1_input_code.txt", "r");
    optab = fopen("pass1_optab.txt", "r");
    symtab = fopen("pass1_symtab.txt", "w");
    intermediate = fopen("pass1_intermediate_code.txt", "w");
    littabFile = fopen("pass1_littab.txt", "w");

    if (!input || !optab || !symtab || !intermediate || !littabFile) {
        printf("Error: Could not open files\n");
        exit(1);
    }

// Process first line (START directive)
fgets(line, sizeof(line), input);
sscanf(line, "%s %s %s", label, opcode, operand);

if (strcmp(opcode, "START") == 0) {
    start = (int)strtol(operand, NULL, 16);
    locctr = start;
    fprintf(intermediate, "%04X\t%s\t%s\t%s\n", locctr, label, opcode, operand);
    printf("%04X\t%s\t%s\t%s\n", locctr, label, opcode, operand);  // Console output
} else {
    locctr = 0;
    start = 0;
}

// Process remaining lines
while (fgets(line, sizeof(line), input)) {
    if (line[0] == '\n' || line[0] == '.') continue;  // Skip empty lines and comments

    label[0] = opcode[0] = operand[0] = '\0';
    sscanf(line, "%s %s %s", label, opcode, operand);

    // Write symbol to SYMTAB
    if (label[0] != '\0' && strcmp(label, "-") != 0) {
        fprintf(symtab, "%s\t%04X\n", label, locctr);
    }

    // Process literals in operand
    if (operand[0] == '=') {
        processLiteral(littab, &literalCount, operand, &current_pool);
    }

    // Handle LTORG directive
if (strcmp(opcode, "LTORG") == 0) {
    fprintf(intermediate, "%04X\t-\tLTORG\t-\n", locctr);
    printf("%04X\t-\tLTORG\t-\n", locctr);  // Console output

    // Assign addresses for literals in the current pool
    locctr = assignLiteralAddresses(littab, literalCount, locctr, current_pool);
    current_pool++;
    continue;
}
   // Handle END directive
if (strcmp(opcode, "END") == 0) {
    fprintf(intermediate, "%04X\t-\tEND\t%s\n", locctr, operand);
    printf("%04X\t-\tEND\t%s\n", locctr, operand);  // Console output

    // Assign addresses for any remaining literals
    locctr = assignLiteralAddresses(littab, literalCount, locctr, current_pool);

    // Write remaining literals after END
    for (int i = 0; i < literalCount; i++) {
        if (littab[i].processed && littab[i].pool_number == current_pool) {
            fprintf(intermediate, "%04X\t-\tBYTE\t%s\n", littab[i].address, littab[i].literal);
            printf("%04X\t-\tBYTE\t%s\n", littab[i].address, littab[i].literal);  // Console output
        }
    }

    break;
}
// Writing literals at their assigned locations after END
for (int i = 0; i < literalCount; i++) {
    if (littab[i].processed && littab[i].address >= locctr) {
        fprintf(intermediate, "%04X\t-\tBYTE\t%s\n", littab[i].address, littab[i].literal);
        printf("%04X\t-\tBYTE\t%s\n", littab[i].address, littab[i].literal);  // Console output
    }
}

    // Write to intermediate file and console
    fprintf(intermediate, "%04X\t%s\t%s\t%s\n", locctr, label, opcode, operand);
    printf("%04X\t%s\t%s\t%s\n", locctr, label, opcode, operand);  // Console output

    // Update LOCCTR
    if (opcode[0] == '+') {  // Format 4 instruction
        locctr += 4;
    } else if (strcmp(opcode, "WORD") == 0) {
        locctr += 3;
    } else if (strcmp(opcode, "RESW") == 0) {
        locctr += 3 * atoi(operand);
    } else if (strcmp(opcode, "RESB") == 0) {
        locctr += atoi(operand);
    } else if (strcmp(opcode, "BYTE") == 0) {
        if (operand[0] == 'C') {
            locctr += strlen(operand) - 3;
        } else if (operand[0] == 'X') {
            locctr += (strlen(operand) - 3) / 2;
        }
    } else {
        // Check OPTAB for opcode
        rewind(optab);
        int found = 0;
        while (fgets(line, sizeof(line), optab)) {
            sscanf(line, "%s", mnemonic);
            if (strcmp(opcode, mnemonic) == 0) {
                found = 1;
                locctr += 3;  // Format 3 instruction
                break;
            }
        }
        if (!found && strcmp(opcode, "BASE") != 0) {
            printf("Error: Invalid opcode %s\n", opcode);
        }
    }
}


    // Write LITTAB
    fprintf(littabFile, "Literal\t\tAddress\tSize\tPool\n");
    fprintf(littabFile, "--------------------------------\n");
    for (int i = 0; i < literalCount; i++) {
        fprintf(littabFile, "%-15s\t%04X\t%d\t%d\n",
                littab[i].literal, littab[i].address,
                littab[i].size, littab[i].pool_number);
    }

    // Close files
    fclose(input);
    fclose(optab);
    fclose(symtab);
    fclose(intermediate);
    fclose(littabFile);

    printf("Program length: %04X\n", locctr - start);
}

int main() {
    passOne();
    return 0;
}
