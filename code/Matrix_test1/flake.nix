{
  description = "Development environment for Raspberry Pi Pico";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs";
  # inputs.prisma-utils.url = "github:VanCoding/nix-prisma-utils";

  outputs =
    {
      self,
      # prisma-utils,
      nixpkgs,
    }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      local-pico-sdk = pkgs.pico-sdk.overrideAttrs (_: {
        withSubmodules = true;
      });
      # picozero = pkgs.python3Packages.buildPythonPackage {
      #   pname = "picozero";
      #   version = "0.4.2";

      #   src = pkgs.fetchPypi {
      #     version = "0.4.2";
      #     pname = "picozero";
      #     sha256 = "sha256-pQe0V6iAPqZfpauZZnva+WjYC/GW83RQS8uoBxjgcHg=";
      #   };
      # };
      # prisma =
      #   (prisma-utils.lib.prisma-factory {
      #     inherit pkgs;
      #     prisma-fmt-hash = "sha256-MQnSmx4+S6lQWyn/l2CccbJZG0uHzb4gJV5luAnDl+A=";
      #     query-engine-hash = "sha256-ttsqP6XJuo/iIDFX2VqyOaRKsvE9qDDk8Q7Y0aDm71s=";
      #     libquery-engine-hash = "sha256-oOuR8XtO3I7NDUtx/JXjzHjBxDEFO8jv3x5CgccMzjc=";
      #     schema-engine-hash = "sha256-3Z76iqOAR5ytdfOkq5XQofnUveXqoqibNjaChGKisiM=";
      #   }).fromNpmLock
      #     ./software/package-lock.json; # <--- path to our package-lock.json file that contains the version of prisma-engines
    in
    {
      devShells.${system}.default = pkgs.mkShell rec {
        buildInputs = with pkgs; [
          # SOFTWARE
          # nodejs
          # pnpm
          # docker
          # docker-compose

          # openssl

          # FIRMWARE
          # Required for MicroPython development
          # micropython
          # thonny

          # python3
          # python3Packages.pip-tools
          # python3Packages.distutils
          # picozero

          # Required for C/C++ RPi Pico development
          # Raspberry Pi Pico SDKs
          local-pico-sdk
          picotool

          # Compilers/interpreters
          # python3
          gcc
          gcc-arm-embedded

          # Needed to make
          ninja
          gnumake
          cmake
          extra-cmake-modules

          # Libraries required for runtime
          libusb1
          libftdi1
          hidapi
          zstd
        ];

        # env = prisma.env;

        shellHook = ''
          export PICO_SDK_PATH="${local-pico-sdk}/lib/pico-sdk"
          export LD_LIBRARY_PATH="${pkgs.stdenv.cc.cc.lib}/lib:${pkgs.lib.makeLibraryPath buildInputs}:$LD_LIBRARY_PATH"
        '';
      };
    };
}
