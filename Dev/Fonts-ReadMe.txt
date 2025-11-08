There are two types of fonts you can make with the XDK, using two fairly different programs.

___  ___      _       ______          _   
|  \/  |     | |      |  ___|        | |  
| .  . | __ _| | _____| |_ ___  _ __ | |_ 
| |\/| |/ _` | |/ / _ \  _/ _ \| '_ \| __|
| |  | | (_| |   <  __/ || (_) | | | | |_ 
\_|  |_/\__,_|_|\_\___\_| \___/|_| |_|\__|

	MakeFont.exe lives in the RXDK Bin folder. You run it from the command line to make bitmap fonts, the simpler method. A command (from Powershell, for example) might look like this:
		.\MakeFont -d "C:\Windows\Fonts" "arial.rsp" "Arial18Normal.bmf"
	The key here is in arial.rsp, which is in this directory. The first 5 lines are the config section, though these lines are optional; this is good, because lines 4 and 5 (AntiAliasLevel and RLEWidth) were giving us trouble, and deleting them makes it work just fine, even if it forgoes compression.
	The remaining section is the unicode for each individual character you want included. In the most Microsoft thing ever, for some reason, the lowercase "g" character (U+0067) must be included, or else the font will not load on the Xbox. In our case, the "default" character is the unicode for a box, which on Forza Motorsport in 2023 we lovingly referred to as "tofu text."


______          _  ___  ___      _             
|  ___|        | | |  \/  |     | |            
| |_ ___  _ __ | |_| .  . | __ _| | _____ _ __ 
|  _/ _ \| '_ \| __| |\/| |/ _` | |/ / _ \ '__|
| || (_) | | | | |_| |  | | (_| |   <  __/ |   
\_| \___/|_| |_|\__\_|  |_/\__,_|_|\_\___|_|   

	FontMaker.exe is quite different. It provides a GUI to work with, and generates two files: .abc and .tmf. The former is technically the font, while the latter is the texture data, and these two must be bundled together with bundler.exe into one .xmf file.