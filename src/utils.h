#ifndef UTILS_H
#define UTILS_H

/// @brief Return codes to be used across the program
typedef enum ResultCode_e
{
    CODE_OK,
    CODE_ERROR,
    CODE_MEMORY_ERROR,
    CODE_LOGIC_ERROR,
    CODE_SYNTAX_ERROR,
    CODE_WRITE_ERROR,
    CODE_READ_ERROR,
    CODE_NOT_SUPPORTED
} ResultCode;

/// @brief Possible boolean values
typedef enum
{
    BOOL_TRUE,
    BOOL_FALSE,
    BOOL_UNKNOWN
} Boolean;

#endif