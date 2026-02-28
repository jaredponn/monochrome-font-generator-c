{ lib, ... }:
{
  perSystem = { config, pkgs, ... }: {
    checks.golden = pkgs.stdenv.mkDerivation {
      name = "integration-golden";
      src = ./.;

      nativeBuildInputs = [ config.packages.default ];

      buildPhase = ''
        	set -eu
                export TTF=${../font-fixtures/JetBrains_Mono/static/JetBrainsMono-Regular.ttf}
        	make check-goldens
      '';

      installPhase = ''
        	touch $out
      '';
    };
  };
}
