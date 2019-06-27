{
  "targets": [
    {
      "target_name": "unncessarily-native-lru",
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
