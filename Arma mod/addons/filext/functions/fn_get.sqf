#include "errorCodes.h"

/*
	Author: Saveliy Tronza

	Description:
	Gets value associated with a key from the memory. Does not read anything from the actual file.
    If currently this file is not open or given key does not exist, returned value is nil.

    Parameter(s):
        _fileName - STRING, file name.
        _key - STRING, key.

	Returns:
	STRING, the value associated with key, or nil;
*/

params [["_fileName", "", [""]], ["_key", "", [""]]];

private _substrings = [];

// Get first part of value
("filext" callExtension ["", ["get", _fileName, _key, 1]]) params ["_substr", "_retCode"]; // 1 means we start getting value from the start of the string

// Common case, value returned immediately
if (_retCode == FILEXT_SUCCESS) exitWith {
    _substr;
};

// If key was not found, return nil, don't print error
if (_retCode == FILEXT_ERROR_KEY_NOT_FOUND) exitWith {
    nil; // Either it doesn't exist or file is not open.
};

// If by this point return code is not FILEXT_GET_MORE_AVAILABLE, it is an error
if (_retCode != FILEXT_GET_MORE_AVAILABLE) exitWith {
    [_retCode, _this] call filext_fnc_error;
    nil;
};

// = = = There is more parts of string available, read and combine them all

_substrings pushBack _substr;

// Read other parts of value
while {_retCode == FILEXT_GET_MORE_AVAILABLE} do {
    ("filext" callExtension ["", ["get", _fileName, _key, 0]]) params ["_substr", "__retCode"]; // 0 means we continue getting parts of the string
    _retCode = __retCode;
    _substrings pushBack _substr;
};

_subStrings joinString "";