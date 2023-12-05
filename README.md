# LightAct-To-Unreal-Link-Plugin
A plugin for Unreal Engine that adds the ability to sync the transform of static meshes from a LightAct save file

## Prerequisites
- Unreal Engine 4.27 or later
- Visual Studio 2022 (2019 might work, not tested)

## How to setup
1. Add the plugin to your Plugins folder, either download or clone as a submodule to your project
2. Close and re-open your project and compile the plugin (developed with Visual Studio 2022)
3. Create a new actor blueprint and make sure the parent class is "ALightActTransformToUnreal"
4. You can now place this actor in your level

## Properties
The actor exposes various properties and buttons in categories:
- LightAct Project
  - Here you can specify the path to your project file
  - Convert the path to a relative path for better collaboration
  - Validate that the path to the file is valid
- Mapping
  - Check Mapping: Logs the object name and ID from your LightAct file and what actor tag it is mapped to, also displays warnings for un-mapped objects
  - Update Mapped Actors: Updates the static meshes location, rotation and scale in the current level from the mapped LightAct object ID's
  - LightAct ID to Unreal Actor Tag Map: This is where you map which LightAct object ID affects which Static Mesh in your Unreal Engine level
- Unreal Engine Level Actors
  - Log Static Mesh Actors with Any Tag: This logs out all the static meshes in your level that has any type of tag. Useful to get a reference when creating the mapping
- Debug
  - Write Debug Log: This enables/disables a more verbose log of what is happening with file reading, updating meshes etc. Can clutter output log, so disabled by default.

## How to use
1. Create a LightAct project:
    - Add a static mesh and place it in your viewport. Save the file.
2. Import in Unreal Engine:
    - Import the same static mesh in Unreal Engine. Place it in your level.
3. Create and add the link blueprint:
    -  See the setup section for details on creating and adding the link blueprint to your level.
4. Add the path to your LightAct project file:
    - Enter the path to your LightAct project file in the path field
5. Validate the path that it works
6. Log the LightAct Object ID's and check your output log
7. On your static mesh in your level, scroll down to the Actor tab, add a tag. For example "Building_1"
8. Log static mesh actors with any tag, you'll see your your static mesh and the tag you entered, Ex "SM_Building1 with tag: Building_1"
9. Connect them using the map. Add an entry to the map. Enter the ID of the object from LightAct and the tag we created. Ex "205 -> Building_1"
10. Press Check Mapping and very it worked
11. Press Update Mapped Actors and your mesh should now be placed exactly like in your LightAct project.


## NOTE!!!
- The plugin *DOES NOT* match the name of the object from LightAct. It matches the ID. This means you can have different names of the object, or even rename them and it will still work. Just dont change the ID number in LightAct.
- This plugin could be used for other purposes, like loading assets from a regular json file. But will need to be modified in that case, or follow LightActs key-format exactly.
- If you fix something or add functionallity. Please open a Pull Request and I'll merge it (after review)!


## License
This plugin is distributed under the [GNU Lesser General Public License v2.1](LICENSE) license. See the [LICENSE](LICENSE) file for details.
