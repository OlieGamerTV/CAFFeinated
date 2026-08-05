<img align="left" src="resource/icon_large.png"><h1>CAFFeinated</h1>
<b>CAFFeinated</b> (initially started as <b>MumboGarage</b>) is a C++ tool made to read the various file formats used by Rare during the Xbox/Xbox 360 era via R1.

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
| Loctext Editor  | A viewer for the localisation text banks (often abbreviated to just loctext) used in various R1 titles. |
| Script Editor  | A viewer for the script format, which handles the setup of a scene in the games based on the Ghoulies codebase. |
| Marker Editor | A viewer for the marker format, which handles more specific object placement of a scene in the games based on the Ghoulies codebase. |
| Save Editor | (Nuts & Bolts only) An editor for the save files created by Banjo-Kazooie: Nuts & Bolts. |
| Vehicle Editor | (Nuts & Bolts only) A (fairly extensive) editor for the vehicle files used by Banjo-Kazooie: Nuts & Bolts. |

## Comments
* Some crashes may appear when using the tool, as the tool is still a big Work in Progress.
  * Please feel free to report any issues (whether it's just any jank or outright crashes) with the tool on the issues page.

## Libraries Used
* GLAD & GLFW
* Dear ImGui
* Native File Dialog Extended
* Squish
* stb_image & stb_image_write
* zlib
