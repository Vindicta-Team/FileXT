#include "errorCodes.h"

/*
Internal, decodes error code and prints error into .rpt file.
*/

params [["_errorCode", 0, [0]], ["_args", [], [[]]]];

private _errorText = switch (_errorCode) do {
    case FILEXT_SUCCESS: {"Success"};
    //case FILEXT_GET_MORE_AVAILABLE: {};
    case FILEXT_ERROR_WRONG_ARG_COUNT: {"Wrong argument count"};
    case FILEXT_ERROR_WRONG_FILE_NAME: {"Wrong file name"};
    case FILEXT_ERROR_WRONG_FUNCTION_NAME: {"Wrong function name"};
    case FILEXT_ERROR_FILE_NOT_OPEN: {"File not open"};
    case FILEXT_ERROR_KEY_NOT_FOUND: {"Key not found"};
    case FILEXT_ERROR_WRITE: {"Generic file write error"};
    case FILEXT_ERROR_READ: {"Generic file read error"};
    case FILEXT_ERROR_WRONG_FILE_FORMAT: {"Wrong file format"};
    case FILEXT_ERROR_BUFFER_TOO_SMALL: {"Buffer too small"};
    default {"Unknown error"};
};

diag_log format ["[FileXT] Error: %1, Code: %2, arguments: %3", _errorText, _errorCode, _args];