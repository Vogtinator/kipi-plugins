/* ============================================================
 *
 * Date        : 2010-04-25
 * Description : A class to hold undo/redo commands
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "gpsundocommand.h"

// local includes:

#include "kipiimagemodel.h"


namespace KIPIGPSSyncPlugin
{

GPSUndoCommand::GPSUndoCommand(QUndoCommand* const parent)
: QUndoCommand(parent)
{
}

void GPSUndoCommand::changeItemData(const bool redoIt)
{
    if (undoList.isEmpty())
        return;

    // get a pointer to the KipiImageModel:
    // TODO: why is the model returned as const?
    KipiImageModel* const imageModel = const_cast<KipiImageModel*>(dynamic_cast<const KipiImageModel*>(undoList.first().modelIndex.model()));
    if (!imageModel)
        return;

    for (int i=0; i<undoList.count(); ++i)
    {
        const UndoInfo& info = undoList.at(i);

        GPSImageItem* const item = static_cast<GPSImageItem*>(imageModel->itemFromIndex(info.modelIndex));

        // TODO: correctly handle the dirty flags
        // TODO: find a way to regenerate tag tree
        GPSDataContainer newData = redoIt ? info.dataAfter : info.dataBefore;
        item->restoreGPSData(newData);
    }
}

void GPSUndoCommand::redo()
{
    changeItemData(true);
}

void GPSUndoCommand::undo()
{
    changeItemData(false);
}

void GPSUndoCommand::addUndoInfo(const UndoInfo& info)
{
    undoList << info;
}

void GPSUndoCommand::UndoInfo::readOldDataFromItem(GPSImageItem* imageItem)
{
    this->dataBefore = imageItem->gpsData();
    this->oldTagList = imageItem->getTagList(); 
}

void GPSUndoCommand::UndoInfo::readNewDataFromItem(GPSImageItem* imageItem)
{
    this->dataAfter = imageItem->gpsData();
    this->newTagList = imageItem->getTagList();
}

}  // namespace KIPIGPSSyncPlugin
