{
  "targets": [
    {
      "target_name": "unlru",
      "defines": [ 'BUILD_W_METADATA', 'NDEBUG'],
      "sources": [
        "src/unlru.cc",
        "src/unlruobject.cc",
        "src/metadataobject.cc",
        "src/lru.cc"
        ]
    }
  ]
}
