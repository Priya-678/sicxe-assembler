# SIC/XE Assembler  
## A Two-Pass Assembler with Literal Support  

## 📌 Overview  
This project implements a **two-pass SIC/XE assembler** in C, supporting all instruction formats (1, 2, 3, and 4) and addressing modes. It features independent literal handling, symbol table generation, and object code generation with proper relocation records.

## ✨ Features  

### ✅ Supported Instruction Formats  
| Format | Description | Examples |  
|--------|-------------|----------|  
| **1**  | 1-byte instructions | `FIX`, `FLOAT` |  
| **2**  | 2-byte instructions | `CLEAR`, `TIXR` |  
| **3**  | 3-byte instructions | `LDA`, `STA` |  
| **4**  | 4-byte extended instructions | `+LDA`, `+STA` |  

### ✅ Addressing Modes  
- **Direct** (`LDA 1000`)  
- **Indirect** (`LDA @BUFFER`)  
- **Immediate** (`LDA #5`)  
- **Indexed** (`LDA 1000,X`)  
- **PC-relative & Base-relative**  

### ✅ Machine-Independent Features  
- **Literals** (`=C'EOF'`, `=X'05'`)  
- **Symbol Definitions** (`LABEL RESW 1`)  
- **Base Register Handling** (`BASE`, `NOBASE`)  

## 📂 Input & Output Files  

### 🔹 Input  
- A valid **SIC/XE assembly program** (`.asm` file)  

### 🔹 Pass 1 Outputs  
| File | Description |  
|------|-------------|  
| `symtab.txt` | Generated Symbol Table (labels & addresses) |  
| `littab.txt` | Generated Literal Table (literals & addresses) |  
| `intermediate.txt` | Intermediate file for Pass 2 |  

### 🔹 Pass 2 Outputs  
| File | Description |  
|------|-------------|  
| `listing.lst` | Assembly listing with addresses & object codes |  
| `object.obj` | Object program (H, T, E records) |  
| `errors.txt` | Errors in source program (if any) |  
| `tables.txt` | Combined Symbol & Literal Table |  

## ⚙️ Implementation Details  

### 🔧 Pass 1  
- **Location Counter (LOCCTR)** → Tracks addresses from `START`  
- **Symbol Table (SYMTAB)** → Stores labels & addresses  
- **Literal Table (LITTAB)** → Collects literals (`=C'EOF'`, `=X'05'`)  
- **Intermediate File** → Expanded source for Pass 2  

### 🔧 Pass 2  
- **Object Code Generation** → Converts to machine code  
- **Listing File** → Source with addresses & object codes  
- **Object Program** → Generates H, T, E records  
- **Error Handling** → Reports syntax/semantic errors  

## 🚀 How to Run?  

### 1️⃣ Compile the Assembler  
```sh
gcc pass1.c -o pass1
gcc pass2.c -o pass2
```
2️⃣ Run Pass 1
```sh
./pass1 input.asm
```
📌 Generates: symtab.txt, littab.txt, intermediate.txt

3️⃣ Run Pass 2
```sh
./pass2 intermediate.txt
```
📌 Generates: listing.lst, object.obj, errors.txt, tables.txt

📌 Literal Handling
Collected in LITTAB during Pass 1

Assigned addresses during LTORG or END

Processed independently in Pass 2

❌ Error Handling
Checks for:

Undefined symbols

Invalid opcodes

Incorrect addressing

Literal errors

Duplicate labels

Errors logged in errors.txt

📁 Project Structure
text
sicxe-assembler/
├── pass1.c           # Pass 1 Implementation
├── pass2.c           # Pass 2 Implementation
├── optab.h           # Opcode Table
├── symtab.h          # Symbol Table
├── littab.h          # Literal Table
├── input.asm         # Sample Input
├── listing.lst       # Sample Output
└── object.obj        # Object File
📜 Notes
✔ Literals processed independently
✔ Supports BASE/NOBASE directives
✔ Full SIC/XE standard compliance

