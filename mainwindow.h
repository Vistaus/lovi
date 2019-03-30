#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

#include <memory>

class LineProvider;
class LogFormatLoader;
class LogModel;

class QTreeView;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void loadLogFormat(const QString& filePath);
    void loadLog(const QString& filePath);

private:
    void setupUi();
    void setupActions();
    void onRowsInserted();
    void reloadLogFormat();

    const std::unique_ptr<LogFormatLoader> mLogFormatLoader;
    std::unique_ptr<LineProvider> mLineProvider;
    std::unique_ptr<LogModel> mLogModel;

    QAction* const mAutoScrollAction;

    QToolBar* const mToolBar;
    QTreeView* const mTreeView;
};

#endif // MAINWINDOW_H
