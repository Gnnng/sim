#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>

#include <string>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    cpu = new CPU();
    ui->setupUi(this);
    ui->splitter1->setStretchFactor(0, 1);
    ui->splitter1->setStretchFactor(1, 3);
    ui->splitter2->setStretchFactor(1, 1);
    ui->splitter2->setStretchFactor(0, 2);

    textEdit = ui->textEditMips;
    sourceEdit = ui->textEditSource;
    codeEdit = ui->textEditCode;
    infoEdit = ui->textEditDebug;
    cpuEdit = ui->textEditStatus;
    memEdit = ui->textEditMemory;
    conEdit = ui->textEditConsole;

    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(ui->actionSaveSource, SIGNAL(triggered()), this, SLOT(saveSource()));
    connect(ui->actionSaveCode, SIGNAL(triggered()), this, SLOT(saveCode()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAssemble, SIGNAL(triggered()), this, SLOT(assemble()));
    connect(ui->actionRun, SIGNAL(triggered()), this, SLOT(singleStep()));
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(stopCPU()));
    connect(ui->actionDisassemble, SIGNAL(triggered()), this, SLOT(disassemble()));
    setCurrentFile("");
    setUnifiedTitleAndToolBarOnMac(true);

    textHighlighter = new Highlighter(textEdit->document());
    sourceHighlighter = new Highlighter(sourceEdit->document());
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
        QString fileName = QFileDialog::getOpenFileName(this, "Open file", "", "Source code (*.s);;Any file (*)");
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(textEdit, curFile);
    }
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    const QString suffix(".s");
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList filters;
    filters << "Source code(*.s)";
    dialog.setNameFilters(filters);
    dialog.setDefaultSuffix(suffix);
    int ret = dialog.exec();
    QStringList files = dialog.selectedFiles();

    if (ret == QDialog::Accepted)
        return saveFile(textEdit, files.at(0));
    return false;
}

bool MainWindow::saveSource() {
    QFileDialog dialog(this);
    const QString suffix(".s");
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList filters;
    filters << "Source code(*.s)";
    dialog.setNameFilters(filters);
    dialog.setDefaultSuffix(suffix);
    int ret = dialog.exec();
    QStringList files = dialog.selectedFiles();

    if (ret == QDialog::Accepted)
        return saveFile(sourceEdit, files.at(0));
    return false;
}

bool MainWindow::saveCode() {
    QFileDialog dialog(this);
    const QString suffix(".o");
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList filters;
    filters << "Source code(*.o)";
    dialog.setNameFilters(filters);
    dialog.setDefaultSuffix(suffix);
    int ret = dialog.exec();
    QStringList files = dialog.selectedFiles();

    if (ret == QDialog::Accepted)
        return saveFile(codeEdit, files.at(0));
    return false;
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("MIPS Simulator"),
            tr("The <b>MIPS Simulator</b> is authored by\nFOUR UNCLES GRP."));
}

void MainWindow::documentWasModified()
{
    setWindowModified(textEdit->document()->isModified());
}

bool MainWindow::maybeSave()
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

void MainWindow::loadFile(const QString &fileName)
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
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(QPlainTextEdit *edit, const QString &fileName)

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
    out << edit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    qDebug() << curFile;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}

QString MainWindow::strippedName(const QString &fullFileName)
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
    qDebug() << "here";

    if (cpuRunning) {
        stopCPU();
    }

    QString codesOut, errorOut, sourceOut;

    // text
    QString mips = textEdit->toPlainText();
    QStringList mipsLines;
    QString mipsLine;
    mipsLines = mips.split("\n");
    multiins *multi;
    multi = new multiins();
//    multiins multi();

    foreach(mipsLine, mipsLines) {
        multi->add(mipsLine.toStdString());
    }
    multi->add("j 0");
    qDebug() << "1";
    // source, code, error
    vector<string> sourceLines = multi->handle();
    vector<string> codeStr, errors;
    codeStr = multi->translate(errors);
    qDebug() << "2";
    // source out
    bool flag = false;
    for(int i = 0; i < sourceLines.size(); i++) {
        if (sourceLines[i].find(".data") == string::npos)
            flag = true;
        if (sourceLines[i].find(".text") == string::npos)
            flag = false;
        if (!flag)
            sourceOut = sourceOut + sourceLines[i].c_str() + '\n';
    }
    // code out
    for(int i = 0; i < codeStr.size(); i++) {
        codesOut = codesOut + codeStr[i].c_str() + '\n';
    }
    // error out
    for(int i = 0; i < errors.size(); i++) {
        errorOut = errorOut + errors[i].c_str() + '\n';
    }
//    qDebug() << "3";
//    qDebug() << codesOut;
//    qDebug() << errorOut;
    // print
    printToEdit(sourceEdit, sourceOut);
    printToEdit(codeEdit, codesOut);
    printToEdit(infoEdit, errorOut);
    // generate machineCode
    machineCode.clear();
    for(int i = 0; i < 512*1024; i++) {
        machineCode.push_back(multi->mem[i].word);
    }
    delete multi;
}

void MainWindow::disassemble(){
    if (cpuRunning) {
        stopCPU();
    }
    QString code = codeEdit->toPlainText();
    QStringList codeLines;
    QString codeLine;
    QString mipsOut;
    QString errorOut;
    codeLines = code.split("\n");
    singleRemips* remips;
    remips = new singleRemips();
    qDebug() << "construct remips";
    foreach(codeLine, codeLines) {
        if (codeLine.toStdString().size() == 0)
            continue;
        mipsOut = mipsOut + remips->translate(remips->num(codeLine.toStdString())).c_str() + "\n";
        qDebug() << "translate done";
    }
    printToEdit(sourceEdit, mipsOut);
    delete remips;
}

void MainWindow::singleStep(){
    if (!cpuRunning) {
        startCPU();
        cpuRunning = true;
    }
    highlightCurrentLine(sourceEdit, cpu->PC/4 + 1);
    highlightCurrentLine(codeEdit, cpu->PC/4 + 1);
    cpu->run(1);
    QString info, pc, regs;
    pc.sprintf("PC\t\t%08x\n", cpu->PC);
    qDebug() << "running PC" << cpu->PC;
    info.append(pc);
    std::string regname[32]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"};
    for(int i = 0; i < 32; i++) {
        regs.sprintf("$%s\treg[%2d]\t%08x\n", regname[i].c_str(), i, cpu->reg[i]);
        info.append(regs);
    }
    printToEdit(cpuEdit, info);

    QString mem, m;
    for(int i = 0; i < maxPrintMem; i+=4) {
//        m.sprintf("%08x\n", cpu->memory.mmu(i, 0, 0));
        m.sprintf("[%08x]\t%08x\t%08x\t%08x\t%08x\n", i << 2, cpu->memory[i], cpu->memory[i+1], cpu->memory[i+2], cpu->memory[i+3]);
        mem.append(m);
    }
    printToEdit(memEdit, mem);

    QString consoleOut;
    consoleOut.append(cpu->video.c_str());
    printToEdit(conEdit, consoleOut);
//    if ((cpu->PC)/4 >= cpu->IR.size) {
//        stopCPU();
//    }
}

void MainWindow::highlightCurrentLine(QPlainTextEdit *edit, int lineCount) {
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    QColor lineColor = QColor(Qt::green).lighter(160);
    if (lineCount == 0) { // lincCount == 0, then clear selection
        edit->setExtraSelections(extraSelections);
        return;
    }
    // lineCount != 0, then highlight below
    QString content = edit->toPlainText();
    int pos = content.size();
    for(int i = 0; i < content.size(); i++) {
        if (content[i] == '\n')
            lineCount--;
        if (lineCount == 0) {
            pos = i;
            break;
        }
    }
    QTextCursor cur = edit->textCursor();
    cur.setPosition(pos);
    edit->setTextCursor(cur);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = edit->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
    edit->setExtraSelections(extraSelections);
}

void MainWindow::highlightCurrentLine(int lineCount) {
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    QColor lineColor = QColor(Qt::green).lighter(160);
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
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textEdit->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
    textEdit->setExtraSelections(extraSelections);
}

void MainWindow::startCPU(){
//    cpu = new CPU();
    cpu->init(machineCode);
}

void MainWindow::stopCPU(){
    QString empty("");
    printToEdit(cpuEdit, empty);
    printToEdit(conEdit, empty);
    printToEdit(memEdit, empty);
    highlightCurrentLine(sourceEdit, 0);
    highlightCurrentLine(codeEdit, 0);
    cpuRunning = false;
//    if (cpu)
//        delete cpu;
}

void MainWindow::printToEdit(QPlainTextEdit *edit, QString &text){
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    edit->setPlainText(text);
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}
