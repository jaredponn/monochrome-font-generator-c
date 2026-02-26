# Top level module to collect all nix files
{ ... }:
{
  imports = [ ./pre-commit.nix ./add-packages-to-checks.nix ];
}
