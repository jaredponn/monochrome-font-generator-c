# Module for copying the `.packages` to the `.checks` s.t. `nix flake check` # will verify that all the `.packages` can be built (recall that `nix flake
# check` verifies that the `.checks` can be built, and doesn't do that for the
# `.packages`)
{ inputs, ... }:
{
  imports = [ inputs.git-hooks-nix.flakeModule ];

  perSystem = { config, pkgs, ... }:
    {
      checks = config.packages;
    };
}
