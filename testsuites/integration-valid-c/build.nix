{ lib, ... }:
{
  perSystem = { config, pkgs, ... }: {
    checks.integration-valid-c = pkgs.stdenv.mkDerivation {
      name = "integration-valid-c";
      dontUnpack = true;

      nativeBuildInputs = [ config.packages.default ];

      buildPhase = ''
        set -e

        monochrome-font-generator-c \
          --width 1024 --height 1024 \
          --file-prefix char_map --name-prefix char_map \
          ${../fixtures/JetBrains_Mono/static/JetBrainsMono-Regular.ttf}

        "$CC" -c -fsyntax-only char_map.tab.c
      '';

      installPhase = ''
        touch "$out"
      '';
    };
  };
}
