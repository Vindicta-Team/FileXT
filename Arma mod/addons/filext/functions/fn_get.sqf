params ["_fileName", "_key"];

private _substrings = [];

("filext" callExtension ["", ["get", _fileName, _key, 1]]) params ["_substr", "_retCode"];
_substrings pushBack _substr;

while {_retCode == 7} do {
    ("filext" callExtension ["", ["get", _fileName, _key, 0]]) params ["_substr", "__retCode"];
    _retCode = __retCode;
    _substrings pushBack _substr;
};

_subStrings joinString "";