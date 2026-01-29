#!/usr/bin/env python
import os
import sys
from pathlib import Path
from SCons.Script import ARGUMENTS, Copy, SConscript
from methods import print_error

target_path = ARGUMENTS.pop("target_path", "demo/addons/GDK/libs/")
libname = ARGUMENTS.pop("libname", "GDK")
gdk_dir = r"C:/Program Files (x86)/Microsoft GDK/251001/"

source_path = [
    os.path.join("godot-cpp", "include", "godot_cpp"),
    os.path.join("godot-cpp", "gen", "include", "godot_cpp")
]

env = SConscript("godot-cpp/SConstruct")

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

# localEnv["build_profile"] = "build_profile.json"

env.Append(CPPPATH=[env.Dir(d) for d in source_path])
env.Replace(gdk_dir=gdk_dir)

def setup_dependency_copy(env):
    libs = []
    can_copy = False

    output = os.path.join(target_path, env['platform'])
    if env["platform"] == "windows":
        grdk_path = Path(os.path.join(env["gdk_dir"], "GRDK"))

        libs += [str(x) for x in grdk_path.joinpath("GameKit", "Lib").rglob("*.dll")]
        libs += [str(x) for x in grdk_path.joinpath("ExtensionLibraries", "Xbox.Services.API.C", "Lib", "x64", "Release").glob("*.dll")]
        libs += [str(x) for x in grdk_path.joinpath("ExtensionLibraries", "Xbox.LibHttpClient", "Redist", "x64").glob("*.dll")]
        libs += [str(x) for x in grdk_path.joinpath("ExtensionLibraries", "Xbox.XCurl.API", "Redist", "x64").glob("*.dll")]

    excludes = [os.path.basename(str(x)) for x in Path(output).glob("*.dll")]
    valid_sources = [str(x) for x in libs if os.path.basename(str(x)) not in excludes]

    can_copy = len(valid_sources) > 0

    if can_copy:
        env.Replace(CopyOutput=output)
        env.Replace(CopyInput=valid_sources)
    return can_copy

env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

grdk_path = Path(os.path.join(env["gdk_dir"], "GRDK"))

source_path += [
    grdk_path.joinpath("GameKit", "Include"),
    grdk_path.joinpath("ExtensionLibraries", "Xbox.Services.API.C", "Include"),
    grdk_path.joinpath("ExtensionLibraries", "Xbox.LibHttpClient", "Include"),
    grdk_path.joinpath("ExtensionLibraries", "Xbox.Services.API.C", "Include", "Xal")
]
env.Append(CPPPATH=source_path)

lib_path = [
    grdk_path.joinpath("GameKit", "Lib"),
    grdk_path.joinpath("GameKit", "Lib", "amd64"),
    grdk_path.joinpath("ExtensionLibraries", "Xbox.Services.API.C", "Lib", "x64", "Release"),
    grdk_path.joinpath("ExtensionLibraries", "Xbox.Services.API.C", "Lib", "x64", "Release", "v142"),
    grdk_path.joinpath("ExtensionLibraries", "Xbox.LibHttpClient", "Lib", "x64")
]
env.Append(LIBPATH=lib_path)

env.Append(LINKFLAGS=[
    "xgameruntime.thunks.lib", 
    "Microsoft.Xbox.Services.GDK.C.Thunks.lib",
    "bcrypt.lib",
    "user32.lib",
    "kernel32.lib",
    "advapi32.lib",
    "libHttpClient.GDK.lib",
    "appnotify.lib"
])

env.Append(CPPDEFINES=["HC_PLATFORM=HC_PLATFORM_GDK", "HC_DATAMODEL=HC_DATAMODEL_LP64"])

target = (
    "{}{}/{}.{}.{}".format(
        target_path, env["platform"], libname, env["platform"], env["target"]
    )
)

target = "{}.{}{}".format(target, env["arch"], env["SHLIBSUFFIX"])

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

library = env.SharedLibrary(target=target, source=sources)

if setup_dependency_copy(env):
    AddPostAction(library, Copy(env["CopyOutput"], env["CopyInput"]))

Default(library)

