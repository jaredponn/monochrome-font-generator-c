# Nix module to collect all the test projects
{ ... }:
{
  imports = [ ./integration-valid-c/build.nix ];
}
