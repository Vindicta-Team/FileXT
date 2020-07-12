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

// Content of function class if we ever want to change it
#define __FNC_CLASS_CONTENT headerType = -1;

class CfgFunctions
{
  class filext
  {
    class functions
    {
      tag = "filext";
	  file = "filext\functions";
      class get { __FNC_CLASS_CONTENT };
	  class set { __FNC_CLASS_CONTENT };
	  class eraseKey { __FNC_CLASS_CONTENT };

	  class open { __FNC_CLASS_CONTENT };
	  class read { __FNC_CLASS_CONTENT };
	  class close { __FNC_CLASS_CONTENT };
	  class write { __FNC_CLASS_CONTENT };

	  class error { __FNC_CLASS_CONTENT };

	  class getFiles { __FNC_CLASS_CONTENT };
	  class deleteFile { __FNC_CLASS_CONTENT };
	  class fileExists { __FNC_CLASS_CONTENT };
	  
	  class test {};
    };
  };
};