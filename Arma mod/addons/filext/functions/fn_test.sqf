_fileName = "__testfile.txt";

"filext" callExtension ["", ["open", _fileName]];


for "_i" from 0 to 20 do {
private _key = format ["key_%1", _i];
private _value = format ["%1 squared is %2", _i, _i*_i];

"filext" callExtension [_value, ["set", _fileName, _key]];

};


"filext" callExtension ["", ["write", _fileName]];