#ifndef BOWLINGITEMMODEL_H
#define BOWLINGITEMMODEL_H

#include <QtCore/QObject>  // Q_OBJECT
#include <QtCore/QStringList>
#include <QtGui/QStandardItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>
#include <QtCore/QList>

#include "qxt/qxtcsvmodel.h"  // QxtCsvModel

class BowlingItemModel : public QStandardItemModel
{
    Q_OBJECT

public:
    BowlingItemModel();

    // read model from CSV
    void readFromCsv(const QString& filename);

    // write model to CSV
    void saveToCsv(const QString& filename);

    // clear model
    void clear();

    // add player to model
    void addPlayer(const QString& name);

    // remove last added player from model
    void removePlayer();

    // get nubmer of players
    int playerCount() const;

    // get player number by row number
    int playerNumber(int row) const;

    // get frame number by column number
    int frameNumber(int column) const;

    enum class RowType
    {
        ThrowResult,
        FrameResult
    };

    // get row type
    RowType rowType(int row) const;

private:
    enum class FrameType
    {
        Open,
        Spare,
        Strike
    };

    // total number of frames per game
    const int kFrameCount = 10;

    // number of rows in model per player
    const int kRowsPerPlayer = 2;

    // total result column number
    const int kResultColumn = 21;

    // create horizontal header labels
    void createHorizontalHeader();

    // get row number with pinfall for player
    int throwsRowNumber(int player) const;

    // get row number with result for player
    int resultsRowNumber(int player) const;

    // get column number for frame number
    int frameColumn(int frame) const;

    // frame result
    struct FrameResult
    {
        FrameType type;  // how the frame ended
        QList<int> pinfall;  // frame pinfall
    };

    // calculate frame result for player (without bonus balls for spare and
    // strike)
    FrameResult calculateResult(int player, int frame) const;

    // calculate bonus points for frame
    int calculateBonus(int player, int frame, FrameType type) const;

    // get numerical value of frame result
    int frameValue(const FrameResult& frameResult) const;

    // get balls number for frame
    int frameBallsCount(int frame) const;

    // QStandardItemModel auxiliary methods
    // append count rows to model
    bool appendRows(int count);

    // remove count rows from end
    bool removeRowsFromEnd(int count);

    // get and set data by row/column (instead of index)
    QVariant data(int row, int column,
                  const QModelIndex& parent = QModelIndex()) const;
    bool setData(int row, int column, const QVariant& value);

    // tracking new results input
    Q_SLOT void onItemChanged(QStandardItem* item);

    // list of player names
    QStringList playersList;
};

#endif  // BOWLINGITEMMODEL_H
