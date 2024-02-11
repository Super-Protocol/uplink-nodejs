{
  'conditions':[
    ['OS=="linux"',{
      'targets':[
        {
          "target_name": "uplink",
          "include_dirs": ["./functions", "./common"],
          "sources":["./libUplink.cc", "./functions/promises_execute.cc", "./functions/promises_complete.cc", "./common/project_operations.cc", "./common/download_operations.cc", "./common/upload_operations.cc", "./common/access_operations.cc", "./common/bucket_operations.cc", "./common/object_operations.cc", "./common/release_objects_helpers.cc"],
          "libraries":["-L/<(module_root_dir)", "-luplinkcv1.7.0-sp"],
          "cflags_cc": ["-fexceptions","-fPIC","-Wno-unknown-pragmas"]
        }
      ]
    }],
    ['OS=="win"',{
      'targets':[
        {
          "target_name": "uplink",
          "include_dirs": ["./functions_win", "./common"],
          "sources":["libUplink_win.cc" , "./functions_win/promises_execute_win.cc", "./functions_win/promises_complete_win.cc", "./common/project_operations.cc", "./common/download_operations.cc", "./common/upload_operations.cc", "./common/access_operations.cc", "./common/bucket_operations.cc", "./common/object_operations.cc", "./common/release_objects_helpers.cc"],
          "copies":[{
            "destination":"<(module_root_dir)/build/Release",
            "files":["<(module_root_dir)/libuplinkcversion.h","<(module_root_dir)/libuplinkcv1.7.0-sp.dll","<(module_root_dir)/libuplinkcv1.7.0-sp.h","<(module_root_dir)/uplink_definitions.h","<(module_root_dir)/uplink_combat.h"]
          }],
          "ldflags":["-Wl,-rpath","build/Release"],
        },
      ]
    }],
    ['OS=="mac"',{
        'targets':[
          {
            "target_name": "uplink",
            'include_dirs': ['<(module_root_dir)', "./functions", "./common"],
            "sources":["./libUplink.cc", "./functions/promises_execute.cc", "./functions/promises_complete.cc", "./common/project_operations.cc", "./common/download_operations.cc", "./common/upload_operations.cc", "./common/access_operations.cc", "./common/bucket_operations.cc", "./common/object_operations.cc", "./common/release_objects_helpers.cc"],
            "link_settings": {
              "libraries":["-L/<(module_root_dir)", "-luplinkcv1.7.0-sp"],
            },
            "cxxflags":["-fPIC"],
            "cflags_cc": ["-fexceptions","-fPIC","-Wno-unknown-pragmas"],
          }
        ]
    }]
  ]
}