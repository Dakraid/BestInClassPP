# BestInClass++
BestInClass++ is a SKSE plugin for Skyrim Legendary Edition (also referred to as Classic) and currently a Work In Progress.

It main feature is improving the BestInClass functionality offered by default in the game, which highlights the best item based on armor rating or damage value. This plugin enhances this functionality by considering more categories and types for armor, weapons, and ammunition. It replaces the vanilla method entirely and disables the original function. 

Later releases are planned to include mod support through keyword lookup and options for choosing between base and modified values. A Skyrim Special Edition release is not planned by me for now, but given the license other developers are welcomed to port it.

## Requirements
- Skyrim Legendary Edition
- Skyrim Script Extender 1.7.3

## Building
The plugin is written and compiled using Visual Studio 2015 using the v140 platform toolset with the target platform being 8.1.
This plugin also makes use of libSkyrim, which originally was developed by Himika and has been extended by me, which can be found here: https://github.com/Dakraid/libSkyrim

On release(-ish) this will be improvement so any manual setup is no longer required.

## Thanks
I want to thank everyone on the Skyrim RE Discord for helping out with researching and advising on implementation. This includes, in no particular order: DavidJCobb, Nukem, Aers, PowerOfThree (po3), Ryan, and everyone else who has helped making this come to fruition.

Special thanks to DavidJCobb, who has provided massive help by helping with the reverse engineering of some of the undefined classes and also has given me a lot of valuable know how. Aers and PowerOfThree also provided significant information and knowledge. Without these three and the others I wouldn't have been able to get this far.

Also thanks to Himika's and the SKSE's teams effort in making these kinds of modifications easier or possible in the first place, and their massive efforts in reverse engineering Skyrim and its engine.
