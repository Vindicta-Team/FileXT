#include "errorCodes.h"

/*
	Author: Saveliy Tronza

	Description:
	Erases key from memory. After that, calls to fnc_get will return nil.

    Parameter(s):
        _fileName - STRING, file name.
        _key - STRING, key. If it already exists, value is overriden.

	Returns:
	BOOL, true on success.
*/

params [["_fileName", "", [""]], ["_key", "", [""]], ["_value", "", [""]]];

private _ret = "filext" callExtension [_value, ["eraseKey", _fileName, _key]];

private _errorCode = _ret#1;
if (_errorCode != FILEXT_SUCCESS) exitWith {
    [_errorCode, _this] call filext_fnc_error;
    false;
};

true;