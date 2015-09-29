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
premake-data:   [
    "windows"   ["windows"      ["gmake" "codelite" "vs2012" "vs2013"]]
    "mac"       ["mac"          ["gmake" "codelite" "xcode4"]]
    "linux"     ["linux32"      ["gmake" "codelite"]]
]


; Inits
platform: lowercase to-string system/platform/1
change-dir system/options/home

delete-dir: func [
    {Deletes a directory including all files and subdirectories.}
    dir [file! url!]
    /local files
][
    if all [
        dir? dir
        dir: dirize dir
        attempt [files: load dir]
    ][
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
][
    none
]
either req-ver = cur-ver [
    print ["== [" cur-ver "] already present, quitting!"]
    quit
][
    print ["== [" req-ver "] needed, current [" cur-ver "]"]
]


; Updates host
if system/options/args [
    host: to-url system/options/args/1
]


; Updates cache
local: rejoin [cache req-ver '.zip]
remote: replace host tag req-ver
either exists? local [
    print ["== [" req-ver "] found in cache!"]
][
    attempt [make-dir/deep cache]
    print ["== [" req-ver "] not in cache"]
    print ["== Downloading [" remote "]" newline "== Please wait!"]
    call reform [to-local-file system/options/boot system/script/path/download.r remote system/options/home/:local]
    while [not exists? local][
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


; Stores version
write cur-file req-ver
print ["== [" req-ver "] installed!"]


; Runs premake
cur-premake: select premake-data platform
premake-path: dirize rejoin [premake-root cur-premake/1]
premake: read premake-path

print ["== Copying [" premake "] to [" build "]"]
write build/:premake read premake-path/:premake

print ["== Generating build files for [" platform "]"]
change-dir build
foreach config cur-premake/2 [
    print ["== Generating [" config "]"]
    call/wait reform [premake config]
]
change-dir system/options/home
