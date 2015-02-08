Binny [![](https://travis-ci.org/x25/binny.svg?branch=master)](https://travis-ci.org/x25/binny)
=====

Binny is an C++ addon for NodeJS that serializes arrays to blobs. Why? Because it up to 10 times faster than built-in JSON for certain cases.

## Installation

```bash
$ npm install binny
```

## Blobs

```js
Binny.pack(["a", "bb", "ccc"])
```

will return:

```
<Buffer: B1 00 03 00 01 61 00 02 62 62 00 03 63 63 63>

B1 - Version
00 03 - Number of blocks
00 01 - Length of next block
61 - "a"
00 02 - Length of next block
62 62 - "bb"
00 03 - Length of next block
63 63 63 - "ccc"
```

Numbers are stored as big-endian uint16.

In current version Binny can process only arrays of ascii/utf8 strings!

## Perfomance

```sh
$ npm run benchmark
```

Binny is slower than JSON for small arrays (less than 5kb), but it rocks for [this kind of data](https://github.com/x25/binny/blob/master/tests/perfomance.js#L31):

```
Binny.pack: 9933ms
JSON.stringify: 45553ms

Binny.unpack: 6703ms
JSON.parse: 38383ms

(faster is better)
```

## Usage

```js
var Binny = require("binny");

var buffer = Binny.pack(["array", "of", "strings"]);
var arr = Binny.unpack(buffer); //["array", "of", "strings"]
```

## Tests

```sh
$ npm test
```

## License
MIT
