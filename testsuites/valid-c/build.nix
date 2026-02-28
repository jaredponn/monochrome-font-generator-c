{ lib, ... }:
{
  perSystem = { config, pkgs, ... }: {
    checks.integration-valid-c = pkgs.stdenv.mkDerivation {
      name = "integration-valid-c";
      dontUnpack = true;

      nativeBuildInputs = [ config.packages.default ];

      buildPhase = ''
        set -eu
        export TTF=${../font-fixtures/JetBrains_Mono/static/JetBrainsMono-Regular.ttf}

        bash ${./run-tests.sh} ${./test-cases}
      '';

      installPhase = ''
        touch "$out"
      '';
    };
  };
}
