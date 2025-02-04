{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachSystem [
      "x86_64-linux"
    ] (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };

        libPath = pkgs.lib.makeLibraryPath [
          pkgs.wayland
          pkgs.xorg.libX11
          pkgs.xorg.libXcursor
          pkgs.xorg.libXext
          pkgs.xorg.libXi
          pkgs.xorg.libXrandr
        ];

        buildDependencies = [
          pkgs.pkg-config

          # build system
          pkgs.cmake
          pkgs.gnumake

          # opengl
          pkgs.libGL.dev

          # wayland
          pkgs.wayland.dev
          pkgs.libxkbcommon.dev

          # xorg
          pkgs.xorg.libX11.dev
          pkgs.xorg.libXcursor.dev
          pkgs.xorg.libXext.dev
          pkgs.xorg.libXi.dev
          pkgs.xorg.libXrandr.dev
        ];
      in {
        devShells.default = pkgs.mkShell {
          buildInputs = [
            # compilers
            pkgs.emscripten
            pkgs.pkgsCross.mingwW64.buildPackages.gcc

            # debugging tools
            pkgs.gdb
            pkgs.renderdoc
            pkgs.valgrind

            # opengl
            pkgs.libGL

            # wayland
            pkgs.wayland
            pkgs.wayland-protocols
            pkgs.libxkbcommon

            # x11
            pkgs.xorg.libX11
            pkgs.xorg.libXcursor
            pkgs.xorg.libXext
            pkgs.xorg.libXi
            pkgs.xorg.libXrandr

            # docs
            pkgs.python3
            pkgs.python312Packages.pygithub
            pkgs.python312Packages.mkdocs-material
            pkgs.python312Packages.mkdocs-git-revision-date-localized-plugin

            (pkgs.python312Packages.buildPythonPackage rec {
              pname = "mkdocs-git-committers-plugin";
              version = "0.2.3";

              src = pkgs.fetchPypi {
                inherit pname version;
                sha256 = "sha256-dxiNiqzBHVIz1pSUNWcOPWVF/7eg4nTVbzLtOYQ1PGE=";
              };
            })
          ];

          nativeBuildInputs = buildDependencies;

          LD_LIBRARY_PATH = libPath;
        };
      }
    );
}
