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
        export TEST_CASES_DIRECTORY=

        bash ${./run-tests.sh} ${./test-cases}
      '';

      installPhase = ''
        touch "$out"
      '';
    };
  };
}
