# D3D11CodeSet
- Updated [framework made by Frank D Luna.](http://www.d3dcoder.net/d3d11.htm)   
  - Tested on Visual Studio 2015 Community Edition.
- Referenced the article by [Alex Jadczak] (https://alexjadczak.wordpress.com/2014/05/18/updating-directx-11-to-windows-8-0-sdk/)
- This repository can be removed if requested.
- **All rights belong to the author**

# Current Status
- Chapter 1 to 13 has been updated

# If it doesn't compile
- DirectXTK.lib file inside Common folder is built on Debug mode, x64. If you have x86 OS, get DirectXTK from [here] (https://github.com/Microsoft/DirectXTK) and build it to get lib file.
- Go to Project Properties -> Linker -> Input. Then add "DirectXTK.lib" in Additional Dependencies
- Copy resource files to Debug folder.
- Go to Project Properties -> Debugging -> Working Directory and set it to -> $(SolutionDir)$(Configuration)\
- If above doesn't work, open the issue with build/compile error messages copy & pasted
