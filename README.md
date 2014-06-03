Binny [![](https://travis-ci.org/x25/binny.png)](https://travis-ci.org/x25/binny)
=====

Binny is an addon for NodeJS that serializes arrays to blobs. Why? Because he do it up to 10 times faster than ``JSON.parse()`` for certain cases.

```bash
$ npm install binny
```

## Blobs

```js
Binny.pack(['a', 'bb', 'ccc'])
```

Will output:

```
<Buffer b1 00 03 00 01 61 00 02 62 62 00 03 63 63 63>

b1 - Version
00 03 - Size of serialized array
00 01 - Length of next block
61 - A
00 02 - Length of next block
62 62 - BB
00 03 - lenght of next block
63 63 63 - CCC
```

In current version Binny can process only arrays of strings.

## Usage

```js
var Binny = require('binny');

var packed = Binny.pack(['array', 'of', 'strings']);
var unpacked = Binny.unpack(packed);
```

## Tests

```sh
$ npm test
```

## Licence
MIT
