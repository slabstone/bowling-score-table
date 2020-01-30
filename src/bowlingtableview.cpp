#include "bowlingtableview.h"

void BowlingTableView::setModel(BowlingItemModel* model)
{
    model_ = model;
    QTableView::setModel(model_);
    QTableView::resizeColumnsToContents();

    QObject::connect(model_,
                     SIGNAL(rowsInserted(QModelIndex, int, int)),
                     SLOT(setSpans(QModelIndex, int, int)));
}

Q_SLOT void BowlingTableView::setSpans(const QModelIndex& parent,
                                          int start, int end)
{
    if (model_->rowType(parent.row()) == BowlingItemModel::RowType::FrameResult)
    {
        // the result is displayed every second row, the result cell is spanned
        // across 2 columns (3 for last frame)
        for (int r = 1; r < model_->rowCount(); r += 2)
        {
            for (int c = 0; model_->frameNumber(c) < 9; c += 2)
            {
                QTableView::setSpan(r, c, 1, 2);
            }
            QTableView::setSpan(r, 18, 1, 3);
        }
    }
}
