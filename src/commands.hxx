#ifndef PELICAN_COMMANDS_HXX
#define PELICAN_COMMANDS_HXX

#include <easyqt/commands.hxx>

namespace pelican {
	EASYQTCOMMAND_GEN_DEF(CommandMediaViewSelectAll);
	EASYQTCOMMAND_GEN_DEF(CommandMediaViewInvertSelection);
	EASYQTCOMMAND_GEN_DEF(CommandMediaShowAreaScaleIncrease);
	EASYQTCOMMAND_GEN_DEF(CommandMediaShowAreaScaleDecrease);
	EASYQTCOMMAND_GEN_DEF(CommandMediaShowAreaScaleFit);
	EASYQTCOMMAND_GEN_DEF(CommandMediaShowAreaScaleOriginalSize);
	EASYQTCOMMAND_GEN_DEF(CommandShowImportDialog)
	
	void addCommands();
}

#endif

