#ifndef BOWLINGTABLEVIEW_H
#define BOWLINGTABLEVIEW_H

#include <QtCore/QModelIndex>
#include <QtGui/QTableView>

#include "qxt/qxtcsvmodel.h"  // QxtCsvModel

#include "bowlingitemmodel.h"  // BowlingItemModel

class BowlingTableView : public QTableView
{
    Q_OBJECT

public:
    using QTableView::QTableView;

    void setModel(BowlingItemModel* model);

private:
    // set result row span across frame (2 columns)
    Q_SLOT void setSpans(const QModelIndex& parent, int start, int end);

    BowlingItemModel* model_ = nullptr;
};

#endif  // BOWLINGTABLEVIEW_H
