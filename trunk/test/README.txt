Intro
----------------------------------------------------------------------------------------------------------------

A testing script for 'amg_codec_tool' project.
Written in Perl. 
Tested/developed with Windows 7 x64, but portability issues were kept in mind. In theory will work with *nix.

How-to:
----------------------------------------------------------------------------------------------------------------

1. Put your 'amg_codec_tool.exe' to the ./bin directory.
2. Open ./test.pl and configure test set
3. Run ./test.pl

Warning:
----------------------------------------------------------------------------------------------------------------

Not recommended to use with SSD disks. It's maybe a paranoia, but I recommend using RAM disk software,


Directory tree structure
----------------------------------------------------------------------------------------------------------------
./bin
./bin/amg_codec_tool.exe - Put your executable here
./bin/runme.bat          - A script to be run prior to 'amg_codec_tool.exe'. May be useful for different things.
./src                    - Test input files
./tmp                    - Contains intermediate and final test results (created automatically)
./test.pl                - Testset launcher
./README.txt             - This document