{ lib, ... }:
{
  perSystem = { config, pkgs, ... }: {
    checks.integration-golden = pkgs.stdenv.mkDerivation {
      name = "integration-golden";
      src = ./.;

      nativeBuildInputs = [ config.packages.default ];

      buildPhase = ''
        set -e

        monochrome-font-generator-c \
          --width 1024 --height 1024 \
          --file-prefix char_map --name-prefix char_map \
          ${../fixtures/JetBrains_Mono/static/JetBrainsMono-Regular.ttf}

        "$CC" -o render "$src" char_map.tab.c
        ./render "Hello, World!" output.pbm
      '';

      installPhase = ''
        mkdir -p "$out"
        cp output.pbm "$out/"
      '';
    };
  };
}
