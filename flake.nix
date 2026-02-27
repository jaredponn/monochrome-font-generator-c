{
  description = "monochrome-font-generator-c";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";

    git-hooks-nix.url = "github:cachix/git-hooks.nix";
    git-hooks-nix.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = inputs@{ flake-parts, ... }: flake-parts.lib.mkFlake { inherit inputs; } {
    imports = [
      ./nix/build.nix
      ./testsuites/build.nix
    ];

    perSystem = { config, pkgs, ... }: {

      packages.default = pkgs.stdenv.mkDerivation {
        pname = "monochrome-font-generator-c";
        version = "0.0.0";

        src = ./.;

        nativeBuildInputs = [ pkgs.cmake ];
        buildInputs = [ pkgs.freetype ];
      };

      devShells.default = pkgs.mkShell {
        packages = [ pkgs.ctags pkgs.pre-commit ];
        inputsFrom = [
          config.packages.default
        ];

        shellHook =
          ''
            ${config.pre-commit.installationScript}
          '';
      };
    };

    systems = [ "x86_64-linux" ];

  };

}
