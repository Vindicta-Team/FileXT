#include "errorCodes.h"

/*
	Author: Saveliy Tronza

	Description:
	Gets value associated with a key from the memory. Does not read anything from the actual file.

    Parameter(s):
        _fileName - STRING, file name.
        _key - STRING, key. If this key does not exist, function returns an empty string.

	Returns:
	STRING, the value associated with key.
*/

params [["_fileName", "", [""]], ["_key", "", [""]]];

private _substrings = [];

// Get first part of value
("filext" callExtension ["", ["get", _fileName, _key, 1]]) params ["_substr", "_retCode"];

if (_retCode != FILEXT_SUCCESS) exitWith {
    [_errorCode, _this] call filext_fnc_error;
    "";
};

_substrings pushBack _substr;

// Read other parts of value
while {_retCode == 7} do {
    ("filext" callExtension ["", ["get", _fileName, _key, 0]]) params ["_substr", "__retCode"];
    _retCode = __retCode;
    _substrings pushBack _substr;
};

_subStrings joinString "";