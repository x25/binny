var Binny = require('./..');

var numIterations = 1000000;

function measure(name, callback, data) {
	console.time(name);

	for (var i = 0; i < numIterations; i++) {
		callback(data);
	}

	console.timeEnd(name);
}

var testBinnyPack = function (data) {
	Binny.pack(data);
};

var testJsonStringify = function (data) {
	JSON.stringify(data);
};

var testBinnyUnpack = function (data) {
	Binny.unpack(data);
};

var testJsonParse = function (data) {
	JSON.parse(data);
};

//For example we have data in the following format:
//['actionName', 'list of uids', '5kb of json-like data']

var payload = {};

for (var i=0;i<100;i++)
	payload['prop' + i] = 'The quick brown fox jumps over the lazy dog';

var arr = [
	'broadCastMessage',
	'100000000000000,100000000000000,100000000000000,100000000000000,100000000000000,100000000000000,100000000000000,100000000000000',
	JSON.stringify(payload)
];

measure('Binny.pack', testBinnyPack, arr);
measure('JSON.stringify', testJsonStringify, arr);

var blob = Binny.pack(arr);
var json = JSON.stringify(arr);

measure('Binny.unpack', testBinnyUnpack, blob);
measure('JSON.parse', testJsonParse, json);
