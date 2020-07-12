#include "errorCodes.h"

/*
	Author: Saveliy Tronza

	Description:
	Checks if given file exists.

    Parameter(s):
        _fileName - STRING, file name.

	Returns:
	BOOL, true if file exists.
*/

params [["_fileName", "", [""]]];

private _ret = "filext" callExtension ["", ["fileExists", _fileName]];
private _errorCode = _ret#1;
_errorCode == FILEXT_SUCCESS;