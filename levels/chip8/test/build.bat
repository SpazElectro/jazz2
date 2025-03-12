@echo off
c8c test.c8c test.asm && asm test.asm test.hex && bin test.hex ../assets/test.asdat
@echo on