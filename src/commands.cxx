#include <easyqt/objectregistry.hxx>

#include "commands.hxx"
#include "mediashowarea.hxx"
#include "mediaview.hxx"

namespace pelican {
	EASYQTCOMMAND_GEN_IMPL(
		MediaViewSelectAllCommand, "mediaview-select-all",
		easyqt::ObjectRegistry::get<MediaView>()->selectAll()
	);
	EASYQTCOMMAND_GEN_IMPL(
		MediaViewInvertSelectionCommand, "mediaview-invert-selection",
		easyqt::ObjectRegistry::get<MediaView>()->invertSelection()
	);
	EASYQTCOMMAND_GEN_IMPL(
		MediaShowAreaScaleIncreaseCommand, "mediashowarea-scale-increase",
		easyqt::ObjectRegistry::get<MediaShowArea>()->scaleIncrease()
	);
	EASYQTCOMMAND_GEN_IMPL(
		MediaShowAreaScaleDecreaseCommand, "mediashowarea-scale-decrease",
		easyqt::ObjectRegistry::get<MediaShowArea>()->scaleDecrease()
	);
	EASYQTCOMMAND_GEN_IMPL(
		MediaShowAreaScaleFitCommand, "mediashowarea-scale-fit",
		easyqt::ObjectRegistry::get<MediaShowArea>()->scaleFit()
	);
	EASYQTCOMMAND_GEN_IMPL(
		MediaShowAreaScaleOriginalSizeCommand, "mediashowarea-scale-original-size",
		easyqt::ObjectRegistry::get<MediaShowArea>()->scaleOriginalSize()
	)
	
	void addCommands() {
		easyqt::addCommands({
			easyqt::ObjectRegistry::get<MediaViewSelectAllCommand>(),
			easyqt::ObjectRegistry::get<MediaViewInvertSelectionCommand>(),
			easyqt::ObjectRegistry::get<MediaShowAreaScaleIncreaseCommand>(),
			easyqt::ObjectRegistry::get<MediaShowAreaScaleDecreaseCommand>(),
			easyqt::ObjectRegistry::get<MediaShowAreaScaleFitCommand>(),
			easyqt::ObjectRegistry::get<MediaShowAreaScaleOriginalSizeCommand>(),
		});
	}
}

