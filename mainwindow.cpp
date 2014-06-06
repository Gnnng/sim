#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include "multiins.h"

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
    connect(ui->actionStop, SIGNAL(trigered()), this, SLOT(stopCPU()));
    setCurrentFile("");
    setUnifiedTitleAndToolBarOnMac(true);

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
    cpu->run(1);
    QString info;
    info.append("PC\t" + QString::number(cpu->PC, 16).toUpper());
    info.append("\n");
    info.append("Regs\n");
    for(int i = 0; i < 32; i++) {
        info = info + "reg[" + QString::number(i) + "]\t" + QString::number(cpu->reg[i], 10) + "\n";
    }
    #ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
    #endif
    cpuEdit->setPlainText(info);
    #ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
    #endif

    QString mem;
    for(int i = 0; i < 10; i++) {
        mem = mem + "0x" + QString::number(i, 16) + "\t" + QString::number(cpu->memory[i], 16) + "\n";
    }
    #ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
    #endif
    memEdit->setPlainText(mem);
    #ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
    #endif

    if (cpu->PC >= cpu->IR.size()) {
        stopCPU();
        cpuRunning = false;
    }
}

void MainWindow::startCPU(){
//    QString fileName("e:\\Qtworkspace\\sim\\ins.coe");
//    QFile file(fileName);
//    if (!file.open(QFile::WriteOnly | QFile::Text)) {
//        QMessageBox::warning(this, tr("Application"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(fileName)
//                             .arg(file.errorString()));
//        return;
//    }

//    QTextStream out(&file);
//#ifndef QT_NO_CURSOR
//    QApplication::setOverrideCursor(Qt::WaitCursor);
//#endif
//    out << codeEdit->toPlainText();
//#ifndef QT_NO_CURSOR
//    QApplication::restoreOverrideCursor();
//#endif
    cpu = new CPU();
    cpu->init(machineCode);
}

void MainWindow::stopCPU(){
    delete cpu;
}
