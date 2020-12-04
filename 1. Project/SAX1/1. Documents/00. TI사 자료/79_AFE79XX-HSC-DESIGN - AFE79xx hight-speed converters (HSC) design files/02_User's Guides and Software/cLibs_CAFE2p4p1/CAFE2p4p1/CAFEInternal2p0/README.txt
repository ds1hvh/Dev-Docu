
Here are the compilation results when compiled with GNU GCC 

####################################################################
~\Afe7920_examples>make
gcc -Wall -I./Afe79xxAPI/Public/Include -I./Afe79xxUser/Include -o Obj/basicFunctions.o -c ./Afe79xxAPI/Public/Src/basicFunctions.c
gcc -Wall -I./Afe79xxAPI/Public/Include -I./Afe79xxUser/Include -o Obj/calibrations.o -c ./Afe79xxAPI/Public/Src/calibrations.c
gcc -Wall -I./Afe79xxAPI/Public/Include -I./Afe79xxUser/Include -o Obj/controls.o -c ./Afe79xxAPI/Public/Src/controls.c
gcc -Wall -I./Afe79xxAPI/Public/Include -I./Afe79xxUser/Include -o Obj/dsaAndNco.o -c ./Afe79xxAPI/Public/Src/dsaAndNco.c
gcc -Wall -I./Afe79xxAPI/Public/Include -I./Afe79xxUser/Include -o Obj/hAgc.o -c ./Afe79xxAPI/Public/Src/hAgc.c
gcc -Wall -I./Afe79xxAPI/Public/Include -I./Afe79xxUser/Include -o Obj/hMacro.o -c ./Afe79xxAPI/Public/Src/hMacro.c
gcc -Wall -I./Afe79xxAPI/Public/Include -I./Afe79xxUser/Include -o Obj/jesd.o -c ./Afe79xxAPI/Public/Src/jesd.c
gcc -Wall -I./Afe79xxAPI/Public/Include -I./Afe79xxUser/Include -o Obj/pap.o -c ./Afe79xxAPI/Public/Src/pap.c
gcc -Wall -I./Afe79xxAPI/Public/Include -I./Afe79xxUser/Include -o Obj/serDes.o -c ./Afe79xxAPI/Public/Src/serDes.c
gcc -Wall -I./Afe79xxAPI/Public/Include -I./Afe79xxUser/Include -o Obj/afeParameters.o -c ./Afe79xxUser/Src/afeParameters.c
gcc -Wall -I./Afe79xxAPI/Public/Include -I./Afe79xxUser/Include -o Obj/baseFunc.o -c ./Afe79xxUser/Src/baseFunc.c
gcc -Wall -I./Afe79xxAPI/Public/Include -I./Afe79xxUser/Include -o Obj/main.o -c ./example/main.c
gcc -o test.out Obj/basicFunctions.o Obj/controls.o Obj/dsaAndNco.o Obj/hAgc.o Obj/hMacro.o Obj/jesd.o Obj/pap.o Obj/serDes.o Obj/afeParameters.o Obj/baseFunc.o Obj/main.o
##############################################################################

modify the TARGET in the Makefile according to the the user. 
Afe79xxUser/src Contains the
1.baseFunc.h 
2.afeParameters.h
Which have to be modifed to suit to the user domain. 

Afe79xxUser/Includes contains 
3.afeCommonMacros.h  

contains NUM_OF_AFE 
and other common macros which have to be carefully adjusted to users conveninece.