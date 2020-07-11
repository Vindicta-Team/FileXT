#include "errorCodes.h"

/*
	Author: Saveliy Tronza

	Description:
	Sets value associated with a key in the memory. Does not write anything to file.

    Parameter(s):
        _fileName - STRING, file name.
        _key - STRING, key. If it already exists, value is overriden.
        _value - STRING, value.

	Returns:
	BOOL, true on success.
*/

params [["_fileName", "", [""]], ["_key", "", [""]], ["_value", "", [""]]];

private _ret = "filext" callExtension [_value, ["set", _fileName, _key]];

private _errorCode = _ret#1;
if (_errorCode != FILEXT_SUCCESS) exitWith {
    [_errorCode, _this] call filext_fnc_error;
    false;
};

true;