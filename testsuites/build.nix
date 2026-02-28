# Nix module to collect all the test projects
{ ... }:
{
  imports = [
    ./valid-c/build.nix
  ];

  perSystem = { config, pkgs, ... }: {
    # A devShell with the code generator
    devShells.testsuites = pkgs.mkShell {
      packages = [ config.packages.default ];
    };
  };
}
