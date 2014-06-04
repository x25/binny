var assert = require('assert');
var Binny = require('./..');

function pack(arr) {
	var size = 3, offset = 3;

	for (var i = 0; i<arr.length; i++) {
		size += 2 + arr[i].length;
	}

	var result = new Buffer(size);

	result.writeUInt8(0xB1, 0);
	result.writeInt16BE(arr.length, 1);

	for (i = 0; i<arr.length; i++) {
		result.writeInt16BE(arr[i].length, offset);

		result.write(arr[i], 2 + offset);
		offset += 2 + arr[i].length;
	}

	return result;
}

function unpack(buffer) {
	var version = buffer.readUInt8(0);

	var num = buffer.readInt16BE(1);

	var result = [];

	for (var i = 0, offset = 3, len; i < num; i++) {
		len = buffer.readInt16BE(offset);
		result.push(buffer.toString('ascii', offset + 2, offset + 2 + len));
		offset += 2 + len;
	}

	return result;
}

function testPack(data) {
	var actual = Binny.pack(data);
	var expected = pack(data);

	assert.deepEqual(actual, expected);
}

function testUnpack(data) {
	var actual = Binny.unpack(data);
	var expected = unpack(data);

	assert.deepEqual(actual, expected);
}

testPack(['a', 'b', 'c']);
testUnpack(new Buffer([0xb1, 0x00, 0x03, 0x00, 0x01, 0x61, 0x00, 0x02, 0x62, 0x62, 0x00, 0x03, 0x63, 0x63, 0x63]));
