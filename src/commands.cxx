#include <easyqt/objectregistry.hxx>

#include "commands.hxx"
#include "mediaimportdialog.hxx"
#include "mediashowarea.hxx"
#include "mediaview.hxx"

namespace pelican {
	EASYQTCOMMAND_GEN_IMPL(
		CommandMediaViewSelectAll, "mediaview-select-all",
		easyqt::ObjectRegistry::get<MediaView>()->selectAll()
	);
	EASYQTCOMMAND_GEN_IMPL(
		CommandMediaViewInvertSelection, "mediaview-invert-selection",
		easyqt::ObjectRegistry::get<MediaView>()->invertSelection()
	);
	EASYQTCOMMAND_GEN_IMPL(
		CommandMediaShowAreaScaleIncrease, "mediashowarea-scale-increase",
		easyqt::ObjectRegistry::get<MediaShowArea>()->scaleIncrease()
	);
	EASYQTCOMMAND_GEN_IMPL(
		CommandMediaShowAreaScaleDecrease, "mediashowarea-scale-decrease",
		easyqt::ObjectRegistry::get<MediaShowArea>()->scaleDecrease()
	);
	EASYQTCOMMAND_GEN_IMPL(
		CommandMediaShowAreaScaleFit, "mediashowarea-scale-fit",
		easyqt::ObjectRegistry::get<MediaShowArea>()->scaleFit()
	);
	EASYQTCOMMAND_GEN_IMPL(
		CommandMediaShowAreaScaleOriginalSize, "mediashowarea-scale-original-size",
		easyqt::ObjectRegistry::get<MediaShowArea>()->scaleOriginalSize()
	)
	EASYQTCOMMAND_GEN_IMPL(
		CommandShowMediaImportDialog, "show-media-import-dialog", 
		easyqt::ObjectRegistry::get<MediaImportDialog>()->show();
	)
	
	void addCommands() {
		easyqt::addCommands({
			easyqt::ObjectRegistry::get<CommandMediaViewSelectAll>(),
			easyqt::ObjectRegistry::get<CommandMediaViewInvertSelection>(),
			easyqt::ObjectRegistry::get<CommandMediaShowAreaScaleIncrease>(),
			easyqt::ObjectRegistry::get<CommandMediaShowAreaScaleDecrease>(),
			easyqt::ObjectRegistry::get<CommandMediaShowAreaScaleFit>(),
			easyqt::ObjectRegistry::get<CommandMediaShowAreaScaleOriginalSize>(),
			easyqt::ObjectRegistry::get<CommandShowMediaImportDialog>(),
		});
	}
}

