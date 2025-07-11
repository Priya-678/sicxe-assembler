SIC/XE Assembler
A Two-Pass Assembler with Literal Support
📌 Overview
This project implements a two-pass SIC/XE assembler in C, supporting all instruction formats (1, 2, 3, and 4) and addressing modes. It features independent literal handling, symbol table generation, and object code generation with proper relocation records.

✨ Features
✅ Supported Instruction Formats
Format	Description
1	1-byte instructions (e.g., FIX, FLOAT)
2	2-byte instructions (e.g., CLEAR, TIXR)
3	3-byte instructions (e.g., LDA, STA)
4	4-byte extended instructions (e.g., +LDA, +STA)
✅ Addressing Modes
Direct (LDA 1000)

Indirect (LDA @BUFFER)

Immediate (LDA #5)

Indexed (LDA 1000,X)


✅ Machine-Independent Features
Literals (=C'EOF', =X'05')

Symbol Definitions (LABEL RESW 1)

Base Register Handling (BASE, NOBASE)

📂 Input & Output Files
🔹 Input
A valid SIC/XE assembly program (.asm file).

🔹 Pass 1 Outputs
File	Description
symtab.txt	Generated Symbol Table (labels & addresses)
littab.txt	Generated Literal Table (literals & addresses)
intermediate.txt	Intermediate file for Pass 2
🔹 Pass 2 Outputs
File	Description
listing.lst	Assembly listing with addresses & object codes
object.obj	Object program (H, T, E records)
errors.txt	Errors (if any) in the source program
tables.txt	Combined Symbol & Literal Table
⚙️ Implementation Details
🔧 Pass 1
Location Counter (LOCCTR) → Tracks addresses from START.

Symbol Table (SYMTAB) → Stores labels & their addresses.

Literal Table (LITTAB) → Collects literals (=C'EOF', =X'05').

Intermediate File → Expanded source for Pass 2.

🔧 Pass 2
Object Code Generation → Converts instructions to machine code.

Listing File → Shows source code with addresses & object codes.

Object Program (H, T, E records) → Executable output.

Error Handling → Reports syntax & semantic errors.

🚀 How to Run?
1️⃣ Compile the Assembler
sh
gcc pass1.c -o pass1
gcc pass2.c -o pass2
2️⃣ Run Pass 1
sh
./pass1 <input.asm>
📌 Generates: symtab.txt, littab.txt, intermediate.txt.

3️⃣ Run Pass 2
sh
./pass2 <intermediate.txt>
📌 Generates: listing.lst, object.obj, errors.txt, tables.txt.

📌 Literal Handling
Literals (=C'EOF', =X'05') are collected in LITTAB.

Assigned addresses during LTORG or END.

Processed independently in Pass 2.

❌ Error Handling
The assembler checks for:

Undefined symbols

Invalid opcodes

Incorrect addressing modes

Literal errors

Duplicate labels

Errors are logged in errors.txt.

📁 Project Structure
text
sicxe-assembler/  
├── pass1.c           # Pass 1 Implementation  
├── pass2.c           # Pass 2 Implementation  
├── optab.h           # Opcode Table  
├── symtab.h          # Symbol Table Functions  
├── littab.h          # Literal Table Functions  
├── input.asm         # Sample Input  
└── object.obj        # Sample Object File  
📜 Notes
✔ Literals are processed independently (not tied to specific instructions).
✔ Base-relative addressing is supported via BASE directive.
✔ Supports all standard SIC/XE features.

