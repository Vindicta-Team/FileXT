/*
	Author: Saveliy Tronza

	Description:
	Returns an array of file names in storage folder.

	Parameter(s):
		none

	Returns:
	ARRAY of strings.
*/

private _retArray = "filext" callExtension ["", ["getFiles"]];
parseSimpleArray (_retArray#0);