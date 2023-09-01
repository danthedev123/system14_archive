;
;    * pageloading.asm
;    * 
;    * ABSTRACT:
;    * 
;    *   -> Loads a Page Map Level 4 (PML4) into the correct register.
;    *   -> C syntax: cr3load(<ptr to pml4>)
;    * 
;    * COPYRIGHT (C) 2023 DanielH
;    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
;    * 
;    * HISTORY
;    *   -> 2023 DanielH created
;    * 


bits 64
section .data
ALIGN 16

section .text
global cr3load
cr3load:
    MOV CR3, RDI

    RET
