
- to fix :
	- create some class that holds all the commands that relate the app and don't anywhere to go like
		- add wallpaper
		- add folder
	- fix the remove menu and everywhere to use the new get_folders methods of wallpaper manager 

- to improve : 
	- add methods to wallpaper manager to access the vector of folders excluding the default folder by returning std::span
		- `get_folders_count`: returns the number of folders excluding the default folder
		- `get_all_folders_count`: returns the number of all folders
		- `get_folders`: returns the list of folders excluding the default folder
		- `get_all_folders`: returns the list of all folders 
		- 
- to add :
	- commands left to add to new ui : load

	