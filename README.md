\# SIC/XE Assembler – Two Pass Implementation



This project is an implementation of a \*\*two-pass assembler for SIC/XE architecture\*\*, supporting all standard instruction formats, addressing modes, and a literal-independent mechanism.



---



\## 🚀 Features



\- Supports all SIC/XE instruction formats: Format 1, 2, 3, 4

\- All addressing modes: immediate, indirect, indexed, extended

\- \*\*Literals handled independently\*\* of instruction order

\- Generates:

&nbsp; - Symbol table

&nbsp; - Literal table

&nbsp; - Intermediate file

&nbsp; - Listing file

&nbsp; - Object code with H, T, M, and E records

&nbsp; - Error file



---



\## 📂 Folder Structure



```bash

📁 oee/

├── pass1.cpp           # Source code for Pass 1

├── pass2.cpp           # Source code for Pass 2

├── input.asm           # Example assembly input

├── symtab.txt          # Symbol Table output

├── littab.txt          # Literal Table output

├── intermediate.txt    # Intermediate file for Pass 2

├── listing.txt         # Listing file with object code

├── object\_code.txt     # Final object program (H, T, M, E records)

├── errors.txt          # Any errors encountered

├── README.md           # Project documentation



⚙️ How it Works

🔁 Pass 1 Responsibilities

Builds Symbol Table and Literal Table



Tracks Location Counter



Creates Intermediate File



Defers literal assignment until LTORG or END directive



Handles literal-independence: literals assigned memory without binding to instructions



⚙️ Pass 2 Responsibilities

Generates Object Code



Resolves addresses using SYMTAB \& LITTAB



Constructs final object program with:



H (Header)



T (Text)



M (Modification)



E (End)



Handles Base-relative addressing



Independent literal processing for modularity



💡 Sample Input and Output

A sample SIC/XE assembly input (input.asm) is provided. Outputs include all tables and files generated during both passes.



🛠️ How to Run

Compile the two programs:



bash

Copy code

g++ pass1.cpp -o pass1

g++ pass2.cpp -o pass2

Run Pass 1:



bash

Copy code

./pass1 input.asm

Run Pass 2:



bash

Copy code

./pass2 intermediate.txt

Check generated files:



symtab.txt



littab.txt



object\_code.txt



listing.txt



errors.txt

