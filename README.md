# printsploitv2-
skidded btw. ty norby (norbyv2 on cord) for the inspiration. this is a v2 of printsploit, but much easier to create.
(ONLY FOR INTEL MACS)

u need the following for this:
- Common Sense
- Latest version of roblox
- Code editor, (Visual Studio Code)
- Homebrew, only to download capstone, download by running: /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
- Capstone, download by running: brew install capstone

Firstly, download dumper.cpp and go to terminal and run:
cd Downloads
g++ -std=c++11 dumper.cpp -o dumper -I/usr/local/include -L/usr/local/lib -lcapstone
./dumper

Once you do that, it will spit out the print offset for you. Copy the VM offset and save it for later.

After this,
