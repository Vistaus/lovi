#include "mainwindow.h"

#include "filelineprovider.h"
#include "logformat.h"
#include "logformatloader.h"
#include "logmodel.h"

#include <QAction>
#include <QDebug>
#include <QToolBar>
#include <QTreeView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mLogFormatLoader(std::make_unique<LogFormatLoader>()) {
    createUi();
    createActions();
}

MainWindow::~MainWindow() {
}

void MainWindow::loadLogFormat(const QString& filePath) {
    mLogFormatLoader->load(filePath);
}

void MainWindow::loadLog(const QString &filePath) {
    auto fileLineProvider = std::make_unique<FileLineProvider>();
    fileLineProvider->setFilePath(filePath);
    mLineProvider = std::move(fileLineProvider);

    mLogModel = std::make_unique<LogModel>(mLineProvider.get());
    mLogModel->setLogFormat(mLogFormatLoader->logFormat());
    connect(mLogModel.get(), &QAbstractItemModel::rowsInserted, this, &MainWindow::onRowsInserted);

    connect(mLogFormatLoader.get(), &LogFormatLoader::logFormatChanged, mLogModel.get(), &LogModel::setLogFormat);

    mTreeView->setModel(mLogModel.get());
}

void MainWindow::createUi() {
    mToolBar = addToolBar(tr("Toolbar"));

    mTreeView = new QTreeView();
    mTreeView->setRootIsDecorated(false);
    setCentralWidget(mTreeView);
}

void MainWindow::createActions() {
    mAutoScrollAction = new QAction("Auto Scroll");
    mAutoScrollAction->setCheckable(true);
    connect(mAutoScrollAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled) {
            mTreeView->scrollToBottom();
        }
    });

    mToolBar->addAction(mAutoScrollAction);
}

void MainWindow::onRowsInserted() {
    if (mAutoScrollAction->isChecked()) {
        mTreeView->scrollToBottom();
    }
}
