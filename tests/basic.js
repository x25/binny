var assert = require('assert');
var Binny = require('./..');

var data = ['a', 'bb', 'ccc'];

var packed = Binny.pack(data);
var unpacked = Binny.unpack(packed);

assert.deepEqual(data, unpacked);
