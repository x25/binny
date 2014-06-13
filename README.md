Binny [![](https://travis-ci.org/x25/binny.png)](https://travis-ci.org/x25/binny)
=====

Binny is an C++ addon for NodeJS that serializes arrays to blobs. Why? Because he do it up to 10 times faster than built-in JSON for certain cases.

```bash
$ npm install binny
```

## Blobs

```js
Binny.pack(['a', 'bb', 'ccc'])
```

will return:

```
<SlowBuffer: B1 00 03 00 01 61 00 02 62 62 00 03 63 63 63>

B1 - Version
00 03 - Number of blocks
00 01 - Length of next block
61 - A
00 02 - Length of next block
62 62 - BB
00 03 - Lenght of next block
63 63 63 - CCC
```

Numbers are stored as big-endian uint16.

In current version Binny can process only arrays of strings!

## Perfomance

```sh
$ npm run benchmark
```

Binny is slower than JSON for small arrays, but he rocks for complex structures:

```
Binny.pack: 9933ms
JSON.stringify: 45553ms

Binny.unpack: 6703ms
JSON.parse: 38383ms

(faster is better)
```

## Usage

```js
var Binny = require('binny');

var buffer = Binny.pack(['array', 'of', 'strings']);
var arr = Binny.unpack(buffer); //['array', 'of', 'strings']
```

## Tests

```sh
$ npm test
```

## License
MIT
