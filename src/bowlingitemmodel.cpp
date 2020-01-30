#include "bowlingitemmodel.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QtDebug>

BowlingItemModel::BowlingItemModel()
{
    createHorizontalHeader();

    QObject::connect(this,
        SIGNAL(itemChanged(QStandardItem*)),
        SLOT(onItemChanged(QStandardItem*)));
}

void BowlingItemModel::readFromCsv(const QString& filename)
{
    clear();

    // reading CSV-table
    QxtCsvModel csvModel(filename, nullptr, false, ';');

    // filling pinfall data
    for (int r = 0; r < csvModel.rowCount(); r++)
    {
        // the first row contains player name
        const QString playerName(csvModel.index(r, 0).data().toString());
        addPlayer(playerName);
        
        for (int c = 1; c < csvModel.columnCount(); c++)
        {
            const int modelRow = throwsRowNumber(r);
            const int modelColumn = c - 1;

            const QString data(csvModel.index(r, c).data().toString());
            setData(modelRow, modelColumn, data);
        }
    }
}

void BowlingItemModel::saveToCsv(const QString& filename)
{
    QxtCsvModel csvModel;
    csvModel.setQuoteMode(QxtCsvModel::DoubleQuote);

    const int playersCount = playersList.size();
    if (playersCount == 0)
    {
        return;
    }

    // CSV-table has additional column with player name
    const int csvColumnCount = QStandardItemModel::columnCount() + 1;

    csvModel.insertRows(0, playersCount);
    csvModel.insertColumns(0, csvColumnCount);

    for (int player = 0; player < playersCount; player++)
    {
        // adding player name to first column
        csvModel.setData(csvModel.index(player, 0), playersList.at(player));

        // adding pinfall
        for (int c = 0; c < QStandardItemModel::columnCount(); c++)
        {
            const QModelIndex index(csvModel.index(player, c + 1));
            csvModel.setData(index, data(throwsRowNumber(player), c));
        }
    }

    csvModel.toCSV(filename, false, ';');
}

void BowlingItemModel::clear()
{
    QStandardItemModel::removeRows(0, QStandardItemModel::rowCount());
    playersList.clear();
}

void BowlingItemModel::addPlayer(const QString& name)
{
    // adding rows to model and player to list
    const bool result = appendRows(kRowsPerPlayer);
    Q_ASSERT(result);
    if (result)
    {
        playersList.append(name);

        // adding vertical headers for player
        QStandardItem* pinsItem =
            new QStandardItem(QString("%1 Pins").arg(name));
        QStandardItemModel::setVerticalHeaderItem(
            QStandardItemModel::rowCount() - 2, pinsItem);

        QStandardItem* resultsItem =
            new QStandardItem(QString("%1 Results").arg(name));
        QStandardItemModel::setVerticalHeaderItem(
            QStandardItemModel::rowCount() - 1, resultsItem);
    }
}

void BowlingItemModel::removePlayer()
{
    // removing rows from model and player from list
    const bool result = removeRowsFromEnd(kRowsPerPlayer);
    Q_ASSERT(result);
    if (result)
    {
        playersList.removeLast();
    }
}

int BowlingItemModel::playerCount() const
{
    return playersList.size();
}

int BowlingItemModel::playerNumber(int row) const
{
    Q_ASSERT(row >= 0);

    // 2 rows per player
    return row / 2;
}

int BowlingItemModel::frameNumber(int column) const
{
    Q_ASSERT(column >= 0 && column <= 2 * kFrameCount);

    // 2 columns per row, the last frame may have 3 columns in case of strike
    if (column < 2 * kFrameCount)
    {
        return column / 2;
    }
    else if (column == 2 * kFrameCount)
    {
        return 9;
    }
    else
    {
        return -1;
    }
}

BowlingItemModel::RowType BowlingItemModel::rowType(int row) const
{
    // pinfall in the first row, frame result in the second
    return row % 2 == 0 ? RowType::ThrowResult : RowType::FrameResult;
}

void BowlingItemModel::createHorizontalHeader()
{
    QStringList labels;
    for (int i = 0; i < kFrameCount; i++)
    {
        for (int j = 0; j < frameBallsCount(i); j++)
        {
            labels << QObject::tr("F%1 B%2").arg(i + 1).arg(j + 1);
        }
    }
    labels << QObject::tr("Total");
    QStandardItemModel::setHorizontalHeaderLabels(labels);
}

int BowlingItemModel::throwsRowNumber(int player) const
{
    Q_ASSERT(player >= 0);
    return player * 2;
}

int BowlingItemModel::resultsRowNumber(int player) const
{
    Q_ASSERT(player >= 0);
    return player * 2 + 1;
}

int BowlingItemModel::frameColumn(int frame) const
{
    Q_ASSERT(frame >= 0 && frame <= 9);
    if (frame >= 0 && frame <= 9)
    {
        return frame * 2;
    }

    return -1;
}

BowlingItemModel::FrameResult BowlingItemModel::calculateResult(int player, int frame) const
{
    Q_ASSERT(player >= 0);
    Q_ASSERT(frame >= 0 && frame < kFrameCount);

    // model coordinates
    const int row = throwsRowNumber(player);
    const int column = frameColumn(frame);

    // creating pinfall list
    FrameType frameType = FrameType::Open;
    QList<int> pinfall;
    for (int i = 0; i < 2; i++)
    {
        const QString ballResult(data(row, column + i).toString());

        if (ballResult.compare("X", Qt::CaseInsensitive) == 0)
        {
            frameType = FrameType::Strike;
            pinfall << 10;
            break;
        }
        else if (ballResult == "/")
        {
            frameType = FrameType::Spare;
            pinfall << (10 - pinfall.at(0));
        }
        else
        {
            pinfall << ballResult.toInt();
        }
    }

    return { frameType, pinfall };
}

int BowlingItemModel::calculateBonus(int player, int frame, FrameType type) const
{
    // calculate the first column to look for bonus balls
    const int nextColumn = [this](int frame, FrameType type)
    {
        const int currentColumn = frameColumn(frame);

        // in case of strike in frame 10 this is the next column, otherwise it's
        // 1 column past it
        if (frame == kFrameCount - 1 && type == FrameType::Strike)
        {
            return currentColumn + 1;
        }
        else
        {
            return currentColumn + 2;
        }
    }(frame, type);

    // amount of bonus balls
    const int bonusBalls = [this](FrameType type)
    {
        switch (type)
        {
        case FrameType::Open:
            return 0;
        case FrameType::Spare:
            return 1;
        case FrameType::Strike:
            return 2;
        default:
            Q_ASSERT(false);
            return 0;
        }
    }(type);

    // finding two next non-zero values in the model and adding them to the list
    QList<int> nextResults;
    const int row = throwsRowNumber(player);
    for (int c = nextColumn;
         c < QStandardItemModel::columnCount() && nextResults.size() < bonusBalls;
         c++)
    {
        const QString currentResult(data(row, c).toString());
        qDebug() << currentResult;
        if (!currentResult.isEmpty())
        {
            if (currentResult.compare("X", Qt::CaseInsensitive) == 0)
            {
                nextResults << 10;
            }
            else if (currentResult == "/")
            {
                nextResults << (10 - nextResults.at(0));
            }
            else
            {
                nextResults << currentResult.toInt();
            }
        }
    }

    // bonus point calculation
    int result = 0;
    for (int i : nextResults)
    {
        result += i;
    }

    return result;
}

int BowlingItemModel::frameValue(const FrameResult& frameResult) const
{
    int result = 0;
    for (int i : frameResult.pinfall)
    {
        result += i;
    }
    return result;
}

int BowlingItemModel::frameBallsCount(int frame) const
{
    // frames 1 to 9 have 2 balls, frame 10 may have 3 balls
    if (frame >= 0 && frame < 9)
    {
        return 2;
    }
    else if (frame == 9)
    {
        return 3;
    }
    else
    {
        Q_ASSERT(false);
        return 0;
    }
}

bool BowlingItemModel::appendRows(int count)
{
    return QStandardItemModel::insertRows(
        QStandardItemModel::rowCount(), count);
}

bool BowlingItemModel::removeRowsFromEnd(int count)
{
    return QStandardItemModel::removeRows(
        QStandardItemModel::rowCount() - count, count);
}

QVariant BowlingItemModel::data(int row, int column, const QModelIndex& parent) const
{
    return QStandardItemModel::data(
        QStandardItemModel::index(row, column, parent));
}

bool BowlingItemModel::setData(int row, int column, const QVariant& value)
{
    return QStandardItemModel::setData(
        QStandardItemModel::index(row, column), value);
}

void BowlingItemModel::onItemChanged(QStandardItem* item)
{
    // recalculate results after pinfall input
    if (rowType(item->row()) == RowType::ThrowResult)
    {
        const int player = playerNumber(item->row());

        // recalculating results for all frames for the sake of simplicity
        int totalResult = 0;
        for (int i = 0; i < kFrameCount; i++)
        {
            const auto result = calculateResult(player, i);

            int resultValue = frameValue(result) +
                calculateBonus(player, i, result.type);

            setData(resultsRowNumber(player), i * 2, resultValue);

            totalResult += resultValue;
        }

        setData(resultsRowNumber(player), kResultColumn, totalResult);
    }
}
