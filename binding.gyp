{
  "targets": [
    {
      "target_name": "nodeRtlsdr",
      "sources": [
        "src/binding.cpp",
        "src/nodeRtlSdr.cpp"
      ],
      "conditions" : [
        [
          'OS!="win"', {
            "libraries" : [
              '-lrtlsdr',
            ],
          }
        ],
        [
          'OS=="win"', {
            "libraries" : [
              '<(module_root_dir)/gyp/lib/librtlsdr.dll.a'
            ]
          }
        ]
      ]
    }
  ]
}
