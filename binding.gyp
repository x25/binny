{
  "targets": [
    {
      "target_name": "binny",
      "cflags": [ '-O2' ],
      "sources": [ "src/binny.cc" ],
      "include_dirs": ["<!(node -e \"require('nan')\")"]
    }
  ]
}
