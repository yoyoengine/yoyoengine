# Release Checklist

## Bump Versions

- `yoyoengine/engine/dist/include/verision.h`
- `yoyoengine/editor/include/editor.h`
- `yoyoengine/engine/src/lua_runtime/runtime.lua`
- `yoyoengine/engine/dist/include/yep.h`

## Create Tag

Create and push a tag on the main branch to freeze the ref.

This will automatically deploy the new docs website.

## Create Release

- Locally, build the release with `yoyoeditor/build_linux.sh`
- rename `build/out/bin/linux` to `build/out/bin/yoyoeditor`
- create tar gzip with `tar -czvf yoyoeditor.tar.gz build/out/bin/yoyoeditor`
- create a release on github with the tag and attach the tar.gz file
- add patch notes
