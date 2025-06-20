
- to fix :
	- create some class that holds all the commands that relate the app and don't anywhere to go like
		- remove wallpaper

- to improve : 
	- replace the wallpaper array with the following structure :
		- a vector of folders each one containing a vector of wallpapers + a speacial folder for containing the wallpapers that are not in any folder
	- for random wallpaper, use the same single vector for all indices, then to get the wallpaper of given index, call `get_wallpaper(index)` 
	- for sequentail wallpaper, set next and previous, use the same single index and call `get_wallpaper(index)` to get the wallpaper at that index (could optimize this by keeping track of the current folder and wallpaper using 2 indices but not worth the effort)
	- change the `get_wallpaper(index)` function to work with the new structure by looping through the folders and decreasing the index by the size of each folder until it's less than the size of the current folder, then return the wallpaper at that index in the current folder
	- change removing folder and adding ones to work with the new structure
	- add the ability to remove wallpapers from some folders


- to add :
	- commands left to add to new ui : load

	