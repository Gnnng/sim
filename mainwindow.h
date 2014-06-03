#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "newmips.h"

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
class QTextBrowser;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();
    void assemble();
    void disassemble();
    void singleStep();

private:
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QPlainTextEdit *textEdit;
    QPlainTextEdit *codeEdit;
    QPlainTextEdit *infoEdit;
    QString curFile;
    Ui::MainWindow *ui;

    INSSET insset;
};

#endif // MAINWINDOW_H
