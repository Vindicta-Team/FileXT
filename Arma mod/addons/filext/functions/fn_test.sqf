/*
A brief test performing read-write operations and verifying the returned results.
Returns true on success.
*/

_fileName = "__filext_test.fxt";

private _keys = [];
private _values = [];

for "_i" from 0 to 20 do {
    _keys pushback (format ["key_%1", _i]);
    _values pushBack (format ["%1 squared is %2", _i, _i*_i]);
};

// Write values
[_fileName] call filext_fnc_open;
{
    [_fileName, _x, _values#_foreachindex] call filext_fnc_set;
} forEach _keys;
[_fileName] call filext_fnc_write;
[_fileName] call filext_fnc_close;

// Read values
[_fileName] call filext_fnc_open;
[_fileName] call filext_fnc_read;
private _valuesRead = [];
{
    _valuesRead pushBack ([_fileName, _x] call filext_fnc_get);
} forEach _keys;
[_fileName] call filext_fnc_close;

diag_log "[FileXT] Test result:";
diag_log format ["  Expected: %1", _values];
diag_log format ["  Returned: %1", _valuesRead];

private _success = _values isEqualTo _valuesRead;

diag_log format ["  Success: %1", _success];

_success