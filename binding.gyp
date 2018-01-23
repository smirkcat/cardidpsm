{
    "targets": [{
        "target_name": "cardparse",
        "conditions": [[
            'OS=="win"', {
                'defines': ['']
            }
        ], [
            'OS=="linux"', {
                'cflags': ['-std=c++11'],
                'cflags_cc': ['-std=c++11']
            }
        ]],
        "sources": [
            "src/psm_check.cpp",
            "src/psm_czn_check.cpp",
            "src/psm_init.cpp",
            "src/addon.cpp",
            "src/psmobject.cpp",
        ],
        'include_dirs': ["src"]
    }]
}