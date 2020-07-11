#include "errorCodes.h"

/*
	Author: Saveliy Tronza

	Description:
	Writes current RAM content associated with this file into the file.

    Parameter(s):
        _fileName - STRING, file name.

	Returns:
	BOOL, true on success.
*/

params [["_fileName", "", [""]]];

private _ret = "filext" callExtension ["", ["write", _fileName]];
private _errorCode = _ret#1;
if (_errorCode != FILEXT_SUCCESS) exitWith {
    [_errorCode, _this] call filext_fnc_error;
    false;
};

true;