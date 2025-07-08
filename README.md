# SelfQLite
I aim on creating a single self-modifying executable binary that runs SQLite and contains the entire db file as part of itself

Why? Because I can *(maybe)*

## Target
- `ELF64` (linux)


## SQLite DB
- The DB is stored at the end of ELF file, right before EOF

