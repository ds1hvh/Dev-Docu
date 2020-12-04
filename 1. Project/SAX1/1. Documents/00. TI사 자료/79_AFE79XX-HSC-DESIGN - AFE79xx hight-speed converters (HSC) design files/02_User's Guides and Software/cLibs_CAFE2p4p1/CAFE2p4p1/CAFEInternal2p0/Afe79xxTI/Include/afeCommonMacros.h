#ifndef COMMON_MACROS_H_
#define COMMON_MACROS_H_
/** @file afeCommonMacros.h
 * 	@brief	This file contains C Macros for different kinds of operations in the AFE function.
*/
/// Number of AFEs controlled by the host. This should be set by the user.
#define NUM_OF_AFE 1
#include "afe79xxTypes.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

/// This C Macro has the operation on what to do when the input parameters to AFE function are invalid. It is not recommended to change its contents.
#define AFE_PARAMS_VALID(args)                                           \
    if (!(args))                                                         \
    {                                                                    \
        afeLogErr("Parameter did not satisfy the condition: %s", #args); \
        return RET_EXEC_FAIL;                                            \
    }

/// This C Macro has the operation on what to do when the AFE ID(afeId) is invalid. It is not recommended to change its contents.
#define AFE_ID_VALIDITY()                           \
    if (afeId >= NUM_OF_AFE)                        \
    {                                               \
        afeLogErr("%s", "device ID out of bounds"); \
        return RET_EXEC_FAIL;                       \
    }

/** This C Macro has the operation on what to do when an SPI driver function fails.<br>
 *  There are two recommended recovery ways for this<br>
 *  Option 1. Resolve the SPI issue, reperform the last SPI operation and continue with the execution.<br>
 *  Option 2. Resolve the SPI issue, close all the open pages using the function closeAllPages(afeId). Call the failed function again.*/
#define AFE_SPI_EXEC(args)                                           \
    if (RET_OK != (args))                                            \
    {                                                                \
        afeLogErr("Execution of function(SPI) failed: %s", #args);   \
        errorStatus |= 1;                                            \
        return RET_EXEC_FAIL;                                        \
    }                                                                \
    else                                                             \
    {                                                                \
        afeLogDbg("Executed function(SPI) successfully: %s", #args); \
    }

/** This C Macro handles what to do when a sub-function call fails. <br>
 * If on fail, RET_EXEC_FAIL is returned, then the function execution is stopped and returns RET_EXEC_FAIL. <br>
 * If only the command "errorStatus |= 1;" is executed, then the main function execution will continue and the main called function will return RET_EXEC_FAIL.*/
#define AFE_FUNC_EXEC(args)                                          \
    if (RET_OK != (args))                                            \
    {                                                                \
        afeLogErr("AFE Function Execution failed: %s", #args);       \
        errorStatus |= 1;                                            \
        return RET_EXEC_FAIL;                                        \
    }                                                                \
    else                                                             \
    {                                                                \
        afeLogDbg("AFE Function Executed successfully: %s ", #args); \
    }

/// This C Macro has the operation on what to do when the AFE MCU MACRO(not C Macro) Ready poll fails. It is not recommended to change its contents.
#define AFE_MACRO_READY_POLL_FAIL(args)                              \
    if (RET_OK != (args))                                            \
    {                                                                \
        afeLogErr("AFE MACRO READY POLL FAILED: %s", #args);         \
        errorStatus |= 1;                                            \
        return RET_EXEC_FAIL;                                        \
    }                                                                \
    else                                                             \
    {                                                                \
        afeLogDbg("AFE MACRO READY Successfully Passed: %s", #args); \
    }

/// This C Macro has the operation on what to do when the AFE MCU MACRO(not C Macro) Done poll fails. It is not recommended to change its contents.
#define AFE_MACRO_DONE_POLL_FAIL(args)                              \
    if (RET_OK != (args))                                           \
    {                                                               \
        afeLogErr("AFE MACRO DONE POLL FAILED: %s", #args);         \
        errorStatus |= 1;                                           \
        return RET_EXEC_FAIL;                                       \
    }                                                               \
    else                                                            \
    {                                                               \
        afeLogDbg("AFE MACRO DONE Successfully Passed: %s", #args); \
    }

/** This C Macro has the operation on what to do when the AFE MCU MACRO(not C Macro) execution fails.<br>
 * * If on fail, RET_EXEC_FAIL is returned, then the function execution is stopped and returns RET_EXEC_FAIL. <br>
 * If only the command "errorStatus |= 1;" is executed, then the main function execution will continue and the main called function will return RET_EXEC_FAIL.*/
#define AFE_MACRO_EXEC_ERROR(args)                                                                               \
    if (AFE_MACRO_NO_ERROR != (args))                                                                            \
    {                                                                                                            \
        if (((AFE_MACRO_ERROR_IN_OPCODE & (args)) != 0) || ((AFE_MACRO_ERROR_OPCODE_NOT_ALLOWED & (args)) != 0)) \
        {                                                                                                        \
            afeLogErr("AFE MACRO 0x%X: ERROR in OPCODE Received", opcode);                                       \
        }                                                                                                        \
        else if ((AFE_MACRO_ERROR_IN_OPERAND & (args)) != 0)                                                     \
        {                                                                                                        \
            afeLogErr("AFE MACRO 0x%X: ERROR in Operand Received", opcode);                                      \
        }                                                                                                        \
        else if ((AFE_MACRO_ERROR_IN_EXECUTION & (args)) != 0)                                                   \
        {                                                                                                        \
            afeLogErr("AFE MACRO 0x%X: ERROR in Execution.", opcode);                                            \
        }                                                                                                        \
        errorStatus |= 1;                                                                                        \
        return RET_EXEC_FAIL;                                                                                    \
    }                                                                                                            \
    else                                                                                                         \
    {                                                                                                            \
        afeLogDbg("AFE MACRO 0x%X: Executed without Error.", opcode);                                            \
    }

#endif
