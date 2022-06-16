# Основы reversing engineering
## Table of contents
1. [Reverse engineering](#r1)
2. [Main reverse engineering tools](#r2)
3. [Difficulties in file analysis](#r3)
4. [Function structure (Listing 1)](#r4)
5. [Working with data structures (Listing 2)](#r5)
6. [Function calling conventions](#r6)
7. [Listing 1 – Example of simple function](#r7)
8. [Listing 2 – Example work with data structure](#r8)
9. [Literature](#r9)



## <a name="r1">Reverse engineering</a>
**Reverse engineering** — research of program (binary executable files) to understand how program works (data structures, algorithms, protocols) with subsequent reproduction of a part of the functionality in the form of source code. Usually reverse engineering applied in case of program author not provided information about structure data types, communication protocols or program uses algorithms without description.   

Currently, the words "reverse engineering" is most often understood as a process in which one group of developers analyzes the machine code of a program, draws up an algorithm for this program, and draws up exhaustive specifications. After receiving the specifications, another group of developers write their own program based on the received specifications or algorithms. In this case, it is possible to obtain, at a minimum, attached applications similar in functionality.

For example: native client implementation for the Skype protocol.

## <a name="r2">Main reverse engineering tools</a>
- Debuggers (x64Dbg, WinDBG)
- Disassemblers (IDA, Radare, ILSpy)
- API-Logger (API Monitor)
- PE-editors (CFF Explorer)
- Resource editors (Resource Hacker)
- Monitoring files, processes, registry (ProcessMonitor)
- Any other additional instruments for PE-file analysis or their behavior
- Instruments for restoring structure of executable files while bypassing code encryption: Scylla, Relox.

## <a name="r3">Difficulties in file analysis</a>
- **Code obfuscation** — bringing the source text or executable code of the program to a form that preserves its functionality, but makes it difficult to analyze, understand the operation algorithms and modify during decompilation. It is mainly used to obfuscate disassemblers (use specific bundles of processor instructions; "garbage" instructions) or obfuscate source code for scripting languages (given obscure names to variables, functions; add unnecessary variables, functions; code is rendered unreadable, etc.)

- **Anti-debugging** — comes down to debugger detection methods. There are a lot of debugger detection methods, they are mainly based on system structures, counting the execution time of commands (functions), on the specific behavior of the system, etc. Anti-debugging tricks can also be based on errors in the debugger (specific to certain debuggers or even versions). When a debugger is found, the program may either crash, or start executing on false branches, or start using incorrect data, etc. The program may react to debugger detections not immediately, but after some time, to make it difficult to find anti-debugging tricks. There are also anti-anti-debugging tricks (for example, the PhantOm plugin for OllyDbg), the essence of which is to hide the presence of the debugger for the program.

- **Packers** — used to reduce the size of the program (UPX). Packers do not aim to protect the program, but only reduce its size, but due to the fact that the original program code becomes available only after unpacking, it is impossible to disassemble the packed file. Therefore, when working with packed files, you need to determine the packer and version and unpack the file. Partially in the definition of the packer, you can use PEiD.

- **Cryptors** — protect the program from disassembly, debugging, modification and any kind of analysis (WinLicense , Themida). There are both UserMode and KernelMode (application protection can be implemented from the driver). This type of protection can encrypt sections or individual functions, prevent files from being dumped, run debuggers, run an application on virtual machines, etc. To analyze encrypted files, you must first remove the protection. This can be done in many ways (any protection can be removed, the only question is time), the easiest way is to remove protection with ready-made solutions (with a script for IDA, a special utility), for this you need to find out the name of the cryptor and its exact version, which can be done using relevant utilities.

## <a name="r4">Function structure (Listing 1)</a>
1. Stack frame – used for allocating memory for local variables, parameter and local variable addressing and stack unwinding.

2. Function parameters addressed via EBP register, parameters placed in stack under EBP, because of this for parameters addressing can be used this formula: 
~~~
addr_of_param_n = EBP + (1 + n) * sizeof(DWORD)
~~~
where  
	n – parameter number starting from 1  
	sizeof(DWORD) – fixed size of parameter (for x32)

*At the address of EBP (after setting the stack frame) is the value of the EBP of the previous stack frame*  

*At address EBP + 4 (for x32) is the return address from the function.*

*Using EBP from the current frame stack, you can find out the return address and EBP of the previous stack frame. This recursive execution allows building the CALL stack of the current thread.*

3. Local variables are addressed through the EBP register, parameters are on the stack above EBP, therefore, subtraction from EBP is used to address parameters. 

## <a name="r5">Working with data structures (Listing 2)</a>
A structure in memory is represented as a contiguous piece of memory in which the elements of the structure are located. The size of a structure is defined as the sum of the size of all structure elements + alignment. The compiler can add structure alignment to optimize access to elements, that is, if `sizeof(char) == 1`, then the compiler can allocate 4 bytes for the char element (on x32), instead of one (actually 1 byte is used). The address of a structure is determined by the address of the first element of the structure. An example of working with structures is given in Listing 2

## <a name="r6">Function calling conventions</a>
- С (CDECL) – parameters passed through stack from right to left. The stack is cleared by the caller.
- STDCALL - parameters are passed through the stack from right to left. The stack is cleared by the caller.
- FASTCALL (Microsoft specific):
    - x32: first 2 arguments passed through register (ECX, EDX)
    - x64: first 4 arguments passed through register (RCX, RDX, R8, R9)  
Other arguments (if exist), passed through stack from right to left.  
- thiscall (Microsoft specific): used to call class methods. Parameters passed through stack from right to left. Stack is cleared by the function itself. Additionally, a pointer to an object (this) is passed through the ECX register. The first argument via ECX and the rest through stack. For x64 – this is the same fastcall, just this is passed first and through RCX.

## <a name="r7">Listing 1 – Example of simple function</a>
<table border="1px solid black">
    <tr>
        <td>C++</td>
        <td>ASM</td>
        <td>Description</td>
    </tr>
    <tr>
        <td>
            int func(int number)</br> 
            {</br> 
        </td>
        <td>
            1. push ebp</br> 
            2. mov ebp,esp</br>
            3. sub esp,08h</br>
            4. push ebx</br>
        </td>
        <td>
            1-2: setting stack frame</br>
            3: allocating space for local variables on stack frame</br>
            4: storing register</br>
        </td>
    </tr>
    <tr>
        <td>
            int delta = 10; </br> 
            int result = 0;</br>
        </td>
        <td>
            5. mov dword ptr[ebp-04h],0Ah</br> 
            6. mov dword ptr [ebp-08h],0</br>
        </td>
        <td>
            5-6: initializing variables</br>
        </td>
    </tr>
    <tr>
        <td>
            result = number + delta;</br>
        </td>
        <td>
            7. mov eax,dword ptr [ebp+08h]</br> 
            8. add eax,dword ptr [ebp-04h]</br>
            9. mov dword ptr [ebp-04h],eax</br>
        </td>
        <td>
            7: copying value from “number” to the eax register</br>
            8: adding to eax value from delta variable</br>
            9: copying operation result in to variable “result”</br>
        </td>
    </tr>
    <tr>
        <td>
            ++result;</br>
        </td>
        <td>
            10. int dword ptr [ebp-04h]</br>
        </td>
        <td>
            10: incrementing variable result for 1</br>
        </td>
    </tr>
    <tr>
        <td>
            return result;</br>
        </td>
        <td>
            11. mov  eax,dword ptr [ebp-04h]</br>
        </td>
        <td>
            11: copying return value in to eax</br>
        </td>
    </tr>
    <tr>
        <td>
            }</br>
        </td>
        <td>
            12. pop  ebx</br> 
            13. mov esp,ebp</br>
            14. pop ebp</br>
            15. ret 4</br>
        </td>
        <td>
            12: restoring ebx register</br>
            13-14: clearing stack frame</br>
            15: clearing stack and returning from function. Result returned in eax.</br>
        </td>
    </tr>
</table>

## <a name="r8">Listing 2 – Example work with data structure</a>
<table border="1px solid black">
    <tr>
        <td>C++</td>
        <td>ASM</td>
        <td>Description</td>
    </tr>
    <tr>
        <td>
            void func_StructA(</br> 
            StructA *st)</br> 
            {</br> 
        </td>
        <td>
            1. push ebp</br> 
            2. mov ebp,esp</br>
        </td>
        <td>
            1-2: setting stack frame</br>
        </td>
    </tr>
    <tr>
        <td>
            st->valInt = 1;</br> 
        </td>
        <td>
            3. mov eax,dword ptr [ebp+08h]</br> 
            4. mov dword ptr [eax],1</br>
        </td>
        <td>
            3: copy structure address in to eax</br>
            4: accessing structure first field</br>
        </td>
    </tr>
    <tr>
        <td>
            st->str = "char string";</br>
        </td>
        <td>
            5. mov dword ptr [eax+4] 13C5850h</br> 
        </td>
        <td>
            5: accessing second field of structure, copying string address to it</br>
        </td>
    </tr>
    <tr>
        <td>
            st->wstr = L"wchar string";</br>
        </td>
        <td>
            6. mov dword ptr [eax+8], 13C5830h</br>
        </td>
        <td>
            6: accessing third field of structure, copying string address to it</br>
        </td>
    </tr>
    <tr>
        <td>
            st->valBool = false;</br>
        </td>
        <td>
            7. mov byte ptr [eax+0Ch],0</br>
        </td>
        <td>
            7: accessing fourth field of structure, setting byte 0 to it</br>
        </td>
    </tr>
    <tr>
        <td>
            }</br>
        </td>
        <td>
            8. mov esp,ebp</br> 
            9. pop ebp</br>
            10. ret </br>
        </td>
        <td>
            8-9: clearing stack frame</br>
            10: returning from function</br>
        </td>
    </tr>
</table>

## <a name="r9">Literature</a>
1.	https://reverseengineering.stackexchange.com   
2.	Dennis Yurichev Reverse Engineering for beginners



