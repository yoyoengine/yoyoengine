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
          ];

          nativeBuildInputs = buildDependencies;

          LD_LIBRARY_PATH = libPath;
        };
      }
    );
}
