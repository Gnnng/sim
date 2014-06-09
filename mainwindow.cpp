#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include "multiins.h"
#include "highlighter.h"
#include <string>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // read instructions
//    init("e:\\Qtworkspace\\sim\\ins.txt", insset);

    ui->setupUi(this);
    textEdit = ui->plainTextEditMips;
    codeEdit = ui->plainTextEditMachine;
    infoEdit = ui->plainTextEditDebug;
    cpuEdit = ui->plainTextEditStatus;
    memEdit = ui->plainTextEditMemory;

    multiins multi;
    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAssemble, SIGNAL(triggered()), this, SLOT(assemble()));
    connect(ui->actionRun, SIGNAL(triggered()), this, SLOT(singleStep()));
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(stopCPU()));
    setCurrentFile("");
    setUnifiedTitleAndToolBarOnMac(true);

    highlighter = new Highlighter(textEdit->document());

    cpuRunning = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile("");
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}
//! [8]

//! [9]
bool MainWindow::save()
//! [9] //! [10]
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}
//! [10]

//! [11]
bool MainWindow::saveAs()
//! [11] //! [12]
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.exec();
    QStringList files = dialog.selectedFiles();

    if (files.isEmpty())
        return false;
    return saveFile(files.at(0));

}
//! [12]

//! [13]
void MainWindow::about()
//! [13] //! [14]
{
   QMessageBox::about(this, tr("MIPS Simulator"),
            tr("The <b>MIPS Simulator</b> is authored by FOUR UNCLES GRP."));
}
//! [14]

//! [15]
void MainWindow::documentWasModified()
//! [15] //! [16]
{
    setWindowModified(textEdit->document()->isModified());
}

//! [40]
bool MainWindow::maybeSave()
//! [40] //! [41]
{
    if (textEdit->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}
//! [41]

//! [42]
void MainWindow::loadFile(const QString &fileName)
//! [42] //! [43]
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
//    statusBar()->showMessage(tr("File loaded"), 2000);
}
//! [43]

//! [44]
bool MainWindow::saveFile(const QString &fileName)
//! [44] //! [45]
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
//    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}
//! [45]

//! [46]
void MainWindow::setCurrentFile(const QString &fileName)
//! [46] //! [47]
{
    curFile = fileName;
//    std::cout << curFile << std::endl;
    qDebug() << curFile;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}
//! [47]

//! [48]
QString MainWindow::strippedName(const QString &fullFileName)
//! [48] //! [49]
{
    return QFileInfo(fullFileName).fileName();
}

int convert(string line) {
    int code = 0;
    for(int i = 0; i < 32; i++) {
        if (line[i] == '1') {
            code |= (1 << (31-i));
        }
    }
    return code;
}

void MainWindow::assemble(){
    if (cpuRunning) {
        stopCPU();
    }

    QString mips = textEdit->toPlainText();
    QStringList mipsLines;
    QString mipsLine;
    int lineNumber = 0;
    QString codesOut;
    QString errorOut;
    mipsLines = mips.split("\n");

    multiins multi;
    machineCode.clear();
    foreach(mipsLine, mipsLines) {
        multi.add(mipsLine.toStdString());
    }
    multi.handle();
    vector<string> results, errors;
    results = multi.translate(errors);

    for(int i = 0; i < results.size(); i++) {
        codesOut = codesOut + results[i].c_str() + '\n';
        if (results[i].size() == 0)
            continue;
        machineCode.push_back(convert(results[i]));
        qDebug() << convert(results[i]);
    }

    for(int i = 0; i < errors.size(); i++) {
        errorOut = errorOut + "Line:" + QString::number(i) + " " + errors[i].c_str() + '\n';
    }


//    foreach(mipsLine, mipsLines) {
//        string codeStr;
//        string error;
//        int code;

//        lineNumber++;
//        qDebug() << "declare finished";

//        if (single(insset, mipsLine.toStdString(), error, codeStr, code) == 0) {
//            codesOut = codesOut + codeStr.c_str() + '\n';
//            machineCode.push_back(*((unsigned int *)&code));
//            machineCode.push_back(code);
//        } else {
//            codesOut = codesOut + codeStr.c_str() + '\n';
//            errorOut = errorOut + "Line:" + QString::number(lineNumber) + " " + error.c_str() + '\n';
//        }

//    }
    qDebug() << codesOut;
    qDebug() << errorOut;
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    codeEdit->setPlainText(codesOut);
    infoEdit->setPlainText(errorOut);
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

//#ifndef QT_NO_CURSOR
//    QApplication::setOverrideCursor(Qt::WaitCursor);
//#endif

//#ifndef QT_NO_CURSOR
//    QApplication::restoreOverrideCursor();
//#endif

}

void MainWindow::disassemble(){

}

void MainWindow::singleStep(){
    if (!cpuRunning) {
        startCPU();
        cpuRunning = true;
    }
    highlightCurrentLine(cpu->PC/4 + 1);
    cpu->run(1);
    QString info, pc, regs;
    pc.sprintf("PC\t\t%08x\n", cpu->PC);
    info.append(pc);
    std::string regname[32]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"};
    for(int i = 0; i < 32; i++) {
        regs.sprintf("$%s\treg[%2d]\t%08x\n", regname[i].c_str(), i, cpu->reg[i]);
        info.append(regs);
//        info = info + "reg[" + QString::number(i) + "]\t" + QString::number(cpu->reg[i], 16) + "\n";
    }
    #ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
    #endif
    cpuEdit->setPlainText(info);
    #ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
    #endif

    QString mem, m;
    for(int i = 0; i < 12; i++) {
        int x = 0;
        x = cpu->memory[i * 4 + 0];
        x = (x << 8) + cpu->memory[i * 4 + 1];
        x = (x << 8) + cpu->memory[i * 4 + 2];
        x = (x << 8) + cpu->memory[i * 4 + 3];
        m.sprintf("[%08x]\t%08x\n", i, x);
        mem.append(m);
//        mem = mem + "0x" + QString::number(i, 16) + "\t" + QString::number(cpu->memory[i], 16) + "\n";
    }
    #ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
    #endif
    memEdit->setPlainText(mem);
    #ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
    #endif
    if ((cpu->PC)/4 >= cpu->IR.size()) {
        stopCPU();
    }
}

void MainWindow::highlightCurrentLine(int lineCount) {
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    QColor lineColor = QColor(Qt::green).lighter(160);

    qDebug() << textEdit->textCursor().position();
    QString content = textEdit->toPlainText();
    int pos = content.size();
    for(int i = 0; i < content.size(); i++) {
        if (content[i] == '\n')
            lineCount--;
        if (lineCount == 0) {
            pos = i;
            break;
        }
    }
    QTextCursor cur = textEdit->textCursor();
    cur.setPosition(pos);
    textEdit->setTextCursor(cur);

//    cur = codeEdit->textCursor();
//    cur.setPosition(lineCount * 32);
//    codeEdit->setTextCursor(cur);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textEdit->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
//    selection.cursor = codeEdit->textCursor();
//    selection.cursor.clearSelection();
//    extraSelections.append(selection);
    textEdit->setExtraSelections(extraSelections);
}

void MainWindow::startCPU(){
    cpu = new CPU();
    cpu->init(machineCode);
    qDebug() << "PC width is" << sizeof(cpu->PC);
}

void MainWindow::stopCPU(){
    #ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
    #endif
    cpuEdit->setPlainText("");
    #ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
    #endif

    cpuRunning = false;
    delete cpu;
}
