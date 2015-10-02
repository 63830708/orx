REBOL [
    title:      "orx setup"
    author:     "iarwain@orx-project.org"
    date:       26-Sep-2015
    file:       %setup.r
]


; Default settings
tag:            "<version>"
host:           rejoin ["https://bitbucket.org/orx/orx-extern/get/" tag ".zip"]
extern:         %extern/
cache:          %cache/
temp:           %.temp/
premake-root:   dirize extern/premake/bin
build:          %code/build
hg-hook:        "update.orx"
hg:             %.hg/
platform-data:  [
    "windows"   ['premake "windows" 'config ["gmake" "codelite" "vs2012" "vs2013"] 'hg %.hg/hgrc                                                                           ]
    "mac"       ['premake "mac"     'config ["gmake" "codelite" "xcode4"         ] 'hg %.hg/.hgrc                                                                          ]
    "linux"     ['premake "linux32" 'config ["gmake" "codelite"                  ] 'hg %.hg/.hgrc 'deps ["freeglut3-dev" "libsndfile1-dev" "libopenal-dev" "libxrandr-dev"]]
]


; Inits
platform: lowercase to-string system/platform/1
if platform = "macintosh" [platform: "mac"]
platform-info: platform-data/:platform

change-dir system/options/home

delete-dir: func [
    {Deletes a directory including all files and subdirectories.}
    dir [file! url!]
    /local files
] [
    if all [
        dir? dir
        dir: dirize dir
        attempt [files: load dir]
    ] [
        foreach file files [delete-dir dir/:file]
    ]
    attempt [delete dir]
]


; Checks version
req-file: %.extern
cur-file: extern/.version
req-ver: read/lines req-file
cur-ver: either exists? cur-file [
    read/lines cur-file
] [
    none
]
either req-ver = cur-ver [
    print ["== [" cur-ver "] already installed, skipping!"]
] [
    print ["== [" req-ver "] needed, current [" cur-ver "]"]


    ; Updates host
    if system/options/args [
        host: to-url system/options/args/1
    ]


    ; Updates cache
    local: rejoin [cache req-ver '.zip]
    remote: replace host tag req-ver
    either exists? local [
        print ["== [" req-ver "] found in cache!"]
    ] [
        attempt [make-dir/deep cache]
        print ["== [" req-ver "] not in cache"]
        print ["== Fetching [" remote "]" newline "== Please wait!"]
        call reform [
            to-local-file system/options/boot
            system/script/path/download.r
            remote
            system/options/home/:local
        ]
        while [not exists? local] [
            prin "."
            wait 0.5
        ]
        print newline
        print ["== [" req-ver "] cached!"]
    ]


    ; Clears current version
    if exists? extern [
        print ["== Deleting [" extern "]"]
        attempt [delete-dir extern]
    ]


    ; Decompresses
    do system/script/path/rebzip.r
    attempt [delete-dir temp]
    print ["== Decompressing [" local "] => [" extern "]"]
    wait 0.5
    unzip/quiet temp local
    wait 0.5
    rename rejoin [temp load temp] extern
    attempt [delete-dir temp]
    print ["== [" req-ver "] installed!"]


    ; Installs premake
    premake-path: dirize rejoin [premake-root platform-info/premake]
    premake: read premake-path
    print ["== Copying [" premake "] to [" build "]"]
    write build/:premake read premake-path/:premake
    if not platform = "windows" [
        call reform ["chmod +x" build/:premake]
    ]


    ; Stores version
    write cur-file req-ver
]


; Runs premake
premake-path: dirize rejoin [premake-root platform-info/premake]
premake: read premake-path
print ["== Generating build files for [" platform "]"]
change-dir build
foreach config platform-info/config [
    print ["== Generating [" config "]"]
    call/wait rejoin ["./" premake " " config]
]
change-dir system/options/home
print ["== You can now build orx in [" build/:platform "]"]


; Mercurial hook
if exists? hg [
    hgrc: platform-info/hg
    hgrc-file: to-string read hgrc

    either find hgrc-file hg-hook [
        print "== Mercurial hook already installed"
    ] [
        print "== Installing mercurial hook"
        write hgrc append hgrc-file rejoin [
            newline
            "[hooks]"
            newline
            hg-hook
            " = "
            to-local-file system/options/boot
            " "
            system/options/script
            newline
        ]
    ]
]


; Done!
if platform = "linux" [
    print newline
    print ["== IMPORTANT - make sure the following libraries are installed on your system:"]
    foreach lib platform-info/deps [print ["==[" lib "]"]]
    print newline
]

print ["== Setup successful!"]
