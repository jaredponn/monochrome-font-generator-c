# Module for setting up git hooks
{ inputs, ... }:
{
  imports = [ inputs.git-hooks-nix.flakeModule ];

  perSystem = { config, pkgs, ... }:
    {
      pre-commit.settings.hooks = {
        # Nix formatting
        nixpkgs-fmt.enable = true;

        # Spelling
        typos.enable = true;
        typos.excludes =
          [ ];
        typos.settings.configPath = builtins.readFile ../.typos.toml;

        # C formatting
        clang-format.enable = true;
        clang-format.excludes = [ ];

        # Markdown formatting
        mdformat.enable = true;

        # YAML formatting
        yamlfmt.enable = true;
        yamlfmt.settings.lint-only = false;
      };

      pre-commit.settings.excludes = [
      ];

    };
}
