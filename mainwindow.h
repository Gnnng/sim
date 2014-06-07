#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "newmips.h"
#include "cpu.h"
#include <vector>
#include "highlighter.h"

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
    void startCPU();
    void stopCPU();
private:
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void highlightCurrentLine(int lineCount);
    QString strippedName(const QString &fullFileName);

    Highlighter *highlighter;
    QPlainTextEdit *textEdit;
    QPlainTextEdit *codeEdit;
    QPlainTextEdit *infoEdit;
    QPlainTextEdit *cpuEdit;
    QPlainTextEdit *memEdit;

    QString curFile;
    QStringList mipsLines, codeLines;
    std::vector<int> machineCode;
    Ui::MainWindow *ui;
    CPU *cpu;
    bool cpuRunning;
    INSSET insset;
};

#endif // MAINWINDOW_H
