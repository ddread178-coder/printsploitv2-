# printsploitv2
## MADE FOR EDUCATIONAL PURPOSES ONLY

skidded btw. ty norby (norbyv2 on cord) for the inspiration. this is a v2 of printsploit, but much easier to create.

u need the following for this:
- Common Sense
- Latest version of roblox
- Code editor, (Visual Studio Code)
- Homebrew, only to download capstone, download by running: /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
- Capstone, download by running: brew install capstone

Firstly, download dumper.cpp and go to terminal and run:

```
cd ~/Downloads

g++ -std=c++11 dumper.cpp -o dumper -I/usr/local/include -L/usr/local/lib -lcapstone
./dumper 
```

Once you do that, it will spit out the print offset for you. Copy the VM offset and save it for later.

After this, download main.cpp and change the offset in the code to the offset that you dumped using the dumper.
Save the file and run this in terminal:

```
cd ~/Downloads

clang++ -arch x86_64 -std=c++17 -dynamiclib main.cpp -o printsploitv2.dylib -lpthread
```

This should compile the code into a binary called printsploitv2.
After that, remove the codesignature on roblox by running the following in terminal:

```
cd ~

codesign --remove-signature /Applications/Roblox.app/Contents/MacOS/RobloxPlayer
```

Now, inject into roblox by running:
```
cd ~/Downloads

DYLD_INSERT_LIBRARIES="$(pwd)/printsploitv2.dylib" /Applications/Roblox.app/Contents/MacOS/RobloxPlayer
```

Now join a game on roblox and then open your browser to:
http://localhost:9999/

Here, it should display an ui in which you can print whatever u want onto the roblox console.
