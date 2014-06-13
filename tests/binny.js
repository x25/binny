var assert = require('assert');
var Binny = require('./..');

function pack(arr) {
	var size = 3, offset = 3, len = [];

	for (var i = 0; i<arr.length; i++) {
		len[i] = Buffer.byteLength(arr[i], 'utf8');
		size += 2 + len[i];
	}

	var result = new Buffer(size);

	result.writeUInt8(0xB1, 0);
	result.writeInt16BE(arr.length, 1);

	for (i = 0; i<arr.length; i++) {
		result.writeInt16BE(len[i], offset);

		result.write(arr[i], 2 + offset);
		offset += 2 + len[i];
	}

	return result;
}

function unpack(buffer) {
	var version = buffer.readUInt8(0);

	var num = buffer.readInt16BE(1);

	var result = [];

	for (var i = 0, offset = 3, len; i < num; i++) {
		len = buffer.readInt16BE(offset);
		result.push(buffer.toString('utf8', offset + 2, offset + 2 + len));
		offset += 2 + len;
	}

	return result;
}

function testBasic() {
	var arr = Binny.unpack(Binny.pack([]));

	assert.equal(arr.length, 0);

	var err = false;

	try {
		Binny.unpack(new Buffer('bad data'))
	} catch (e) {
		err = true;
	}

	assert.equal(err, true);
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

testBasic();
testPack(['abc', 'я']);
testUnpack(new Buffer([0xb1, 0x00, 0x02, 0x00, 0x03, 0x61, 0x62, 0x63, 0x00, 0x02, 0xd1, 0x8f]));
