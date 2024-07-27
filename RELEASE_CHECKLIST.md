# Release Checklist

## Bump Versions

- `yoyoengine/engine/dist/include/yoyoengine/engine.h`
- `yoyoengine/editor/include/editor.h`
- `yoyoengine/install.sh`

## Create Tag

Create and push a tag on the main branch to freeze the ref.

This will automatically deploy the new docs website.

## Create Release

- Locally, build the release with `yoyoeditor/build_linux.sh`
- rename `build/out/bin/linux` to `build/out/bin/yoyoeditor`
- create tar gzip with `tar -czvf yoyoeditor.tar.gz build/out/bin/yoyoeditor`
- create a release on github with the tag and attach the tar.gz file, as well as the newest copy of `yoyoengine/install.sh`
