var Binny = require('./..');

var numIterations = 1000000;

function measure(name, callback, data) {
  var start = new Date();
  callback(data);
  console.log(name + ':', new Date() - start, 'ms');
}

var testBinnyPack = function (data) {
	for (var i = 0; i<numIterations; i++) {
		Binny.pack(data);
	}
};

var testJsonStringify = function (data) {
	for (var i = 0; i<numIterations; i++) {
		JSON.stringify(data);
	}
};

var testBinnyUnpack = function (data) {
	for (var i = 0; i<numIterations; i++) {
		Binny.unpack(data);
	}
};

var testJsonParse = function (data) {
	for (var i = 0; i<numIterations; i++) {
		JSON.parse(data);
	}
};

var arr = [
	'100000000000000,100000000000000,100000000000000,100000000000000,100000000000000,100000000000000,100000000000000,100000000000000',
	'padding padding padding padding padding padding padding',
	'{"padding1":"padding padding padding","padding2":"padding padding padding","padding3":{"padding1":"padding padding padding","padding2":{"padding1":100000000000000,"padding2":100000000000000,"padding3":100000000000000,"padding4":"padding padding padding","padding5":"padding padding padding","padding6":null,"padding7":null,"padding8":[],"padding9":[],"padding10":{"padding1":100000000000000,"padding2":100000000000000,"padding3":100000000000000,"padding4":"padding padding padding","padding5":"padding padding padding","padding6":null,"padding7":null,"padding8":[],"padding9":[]}},"padding3":{"padding1":100000000000000,"padding2":100000000000000,"padding3":100000000000000,"padding4":"padding padding padding","padding5":"padding padding padding","padding6":null,"padding7":null,"padding8":[],"padding9":[],"padding10":{"padding1":100000000000000,"padding2":100000000000000,"padding3":100000000000000,"padding4":"padding padding padding","padding5":"padding padding padding","padding6":null,"padding7":null,"padding8":[],"padding9":[]}},"padding4":{"padding1":100000000000000,"padding2":100000000000000,"padding3":100000000000000,"padding4":"padding padding padding","padding5":"padding padding padding","padding6":null,"padding7":null,"padding8":[],"padding9":[],"padding10":{"padding1":100000000000000,"padding2":100000000000000,"padding3":100000000000000,"padding4":"padding padding padding","padding5":"padding padding padding","padding6":null,"padding7":null,"padding8":[],"padding9":[]}},"padding5":{"padding1":100000000000000,"padding2":100000000000000,"padding3":100000000000000,"padding4":"padding padding padding","padding5":"padding padding padding","padding6":null,"padding7":null,"padding8":[],"padding9":[],"padding10":{"padding1":100000000000000,"padding2":100000000000000,"padding3":100000000000000,"padding4":"padding padding padding","padding5":"padding padding padding","padding6":null,"padding7":null,"padding8":[],"padding9":[]}},"padding6":{"padding1":100000000000000,"padding2":100000000000000,"padding3":100000000000000,"padding4":"padding padding padding","padding5":"padding padding padding","padding6":null,"padding7":null,"padding8":[],"padding9":[],"padding10":{"padding1":100000000000000,"padding2":100000000000000,"padding3":100000000000000,"padding4":"padding padding padding","padding5":"padding padding padding","padding6":null,"padding7":null,"padding8":[],"padding9":[]}},"padding7":{"padding1":100000000000000,"padding2":100000000000000,"padding3":100000000000000,"padding4":"padding padding padding","padding5":"padding padding padding","padding6":null,"padding7":null,"padding8":[],"padding9":[],"padding10":{"padding1":100000000000000,"padding2":100000000000000,"padding3":100000000000000,"padding4":"padding padding padding","padding5":"padding padding padding","padding6":null,"padding7":null,"padding8":[],"padding9":[]}}}}'
];

measure('Binny.pack', testBinnyPack, arr);
measure('JSON.stringify', testJsonStringify, arr);

var blob = Binny.pack(arr);
var json = JSON.stringify(arr);

measure('Binny.unpack', testBinnyUnpack, blob);
measure('JSON.parse', testJsonParse, json);
