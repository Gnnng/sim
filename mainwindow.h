#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

#include "single_remips.h"
#include "newmips.h"
#include "cpu.h"
#include "highlighter.h"
#include "multiins.h"
#include "mmu.h"


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
    static const int maxPrintMem = (1 << 16);
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    bool saveSource();
    bool saveCode();
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
    bool saveFile(QPlainTextEdit *edit, const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void highlightCurrentLine(int lineCount);
    void highlightCurrentLine(QPlainTextEdit *edit, int lineCount);
    void printToEdit(QPlainTextEdit *edit, QString &text);
    QString strippedName(const QString &fullFileName);

    Highlighter *textHighlighter;
    Highlighter *sourceHighlighter;
    QPlainTextEdit *textEdit;
    QPlainTextEdit *sourceEdit;
    QPlainTextEdit *codeEdit;
    QPlainTextEdit *infoEdit;
    QPlainTextEdit *cpuEdit;
    QPlainTextEdit *memEdit;
    QPlainTextEdit *conEdit;
    QString curFile;
    QStringList mipsLines, codeLines;
    std::vector<int> machineCode;
    Ui::MainWindow *ui;
    CPU *cpu;
    bool cpuRunning;
    INSSET insset;
};

#endif // MAINWINDOW_H
