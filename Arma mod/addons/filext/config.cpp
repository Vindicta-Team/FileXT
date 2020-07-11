class CfgPatches
{
	class filext
	{
		// Meta information for editor
		name = "filext";
		author = "Sparker";
		url = "https://github.com/Vindicta-Team/filext";

		// Minimum compatible version. When the game's version is lower, pop-up warning will appear when launching the game.
		requiredVersion = 1.60; 
		// Required addons, used for setting load order.
		// When any of the addons is missing, pop-up warning will appear when launching the game.
		requiredAddons[] = { "A3_Functions_F" };
		// List of objects (CfgVehicles classes) contained in the addon. Important also for Zeus content (units and groups) unlocking.
		units[] = {};
		// List of weapons (CfgWeapons classes) contained in the addon.
		weapons[] = {};
	};
};

class CfgFunctions
{
  class filext
  {
    class functions
    {
      tag = "filext";
	  file = "filext\functions";
      class get { headerType = -1; };
	  class test {};
    };
  };
};