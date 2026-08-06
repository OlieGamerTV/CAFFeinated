<img align="left" src="resource/icon_large.png"><h1>CAFFeinated</h1>
<b>CAFFeinated</b> (initially started as <b>MumboGarage</b>) is a C++ tool made to read the various file formats used by Rare during the Xbox/Xbox 360 era via R1.

## Features
Currently supports the parsing and exporting from the following games:
| Game  | Extent |
| ------------- | ------------- |
| Grabbed by the Ghoulies  | Supports the *.bnl files contained in <b>bundles/</b> and the on-demand files in <b>demand/</b>, <b>font/</b> & <b>loctext/</b> |
| Conker: Live & Reloaded  | Supports the <b>aid/text/</b> and <b>aid/texture/</b> *.bin files |
| Kameo: Elements of Power | Supports the <b>kameoStringsstripped.str</b> file and *.mdl files, along with the bundle files used in the .lvl files |
| Perfect Dark Zero | Supports the various *.pkg files in <b>PackageData/</b> and the various *.str files in the <b>aid/strings/</b> subfolders |
| Viva Pinata | Supports the *.bnl files contained in <b>bundles/</b> as well as some support for the debug package in <b>debug/</b> |
| Viva Pinata: Trouble in Paradise | Supports the *.bnl files contained in <b>bundles/</b> |
| Banjo-Kazooie: Nuts & Bolts | Supports the uncompressed bundle files in <b>Bundle/4f/</b>, streamed bundles in <b>Bundle/50/</b> and loctext files in <b>Debug/11/</b> & <b>loctext/</b> |

## Usage
This tool does somewhat expect that you have a basic understanding of the files you are looking at beforehand.<br/>
To open a file with the executable, go <b>File -> Open</b>, there will be a list of available options to pick from. 
| File Option  | Description |
| ------------- | ------------- |
| R1 Bundle File  | This is the most common format found, defined by its magic of "CAFF".<br/>The currently supported versions are V0026, V0031 and V0036.  |
| Ghoulies Files  | These are exclusively for Grabbed by the Ghoulies.<br/>Two options are present, the bundle format and on-demand format. |
| Viva Pinata DB Files | These are found in the files for Viva Pinata.<br/>It relates to the three files found in <b>\\Beta\\debug\\</b> for VP and <b>\\Beta\\packed\\</b> for TiP. |
| Rare Package File | These are used in Rare Replay with the extension .rpk / .rpk.gz |
----
A few tool windows are present in this, for different purposes.
| Tools Option  | Description |
| ------------- | ------------- |
| Loctext Editor  | A viewer for the localisation text banks (often abbreviated to just loctext) used in various R1 titles.<br/>Currently limited to <b>LSBL</b>/<b>LBSL</b> |
| Script Editor  | A viewer for the script format, which handles the setup of a scene in the games based on the Ghoulies codebase. |
| Marker Editor | A viewer for the marker format, which handles more specific object placement of a scene in the games based on the Ghoulies codebase. |
| Save Editor | (Nuts & Bolts only) An editor for the save files created by Banjo-Kazooie: Nuts & Bolts. |
| Vehicle Editor | (Nuts & Bolts only) A (fairly extensive) editor for the vehicle files used by Banjo-Kazooie: Nuts & Bolts. |

## Known Issues
* Some crashes may appear when using the tool, as the tool is still a big Work in Progress.
  * Please feel free to report any issues (whether it's just any jank or outright crashes) with the tool on the issues page.
* The tool does not currently support decompressing files compressed via <b>xbcompress</b>, nor does it currently support the DB files from TiP.

## Contact
If you need to contact me regarding anything, you can contact me via Discord (@oliegamertv, I'm most active there) or the Issues tab.

## Disclaimer
<b>This tool is intended for educational and research purposes only. Use at your own discretion.</b><br/>
Distribution or usage of ripped/modified game files may violate Microsoft's Terms of Service.

## Libraries Used
* GLAD & GLFW
* Dear ImGui
* Native File Dialog Extended
* Squish
* stb_image & stb_image_write
* zlib
