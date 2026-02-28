{ lib, ... }:
{
  perSystem = { config, pkgs, ... }: {
    checks.golden = pkgs.stdenv.mkDerivation {
      name = "integration-golden";
      src = ./.;

      nativeBuildInputs = [ config.packages.default ];

      buildPhase = ''
        set -e

      '';

      installPhase = ''
        mkdir -p "$out"
        cp output.pbm "$out/"
      '';
    };
  };
}
