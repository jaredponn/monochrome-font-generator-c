# Nix module to collect all the test projects
{ ... }:
{
  imports = [
    ./valid-c/build.nix
  ];
}
