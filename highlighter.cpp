#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    instructionFormat.setForeground(Qt::darkBlue);
    instructionFormat.setFontWeight(QFont::Bold);

    QStringList instructionPatterns;
    QString ins[] = {"add", "addi", "j", "jr", "jal", "beq", "bne", "sw", "lw"};
    for(int i = 0; i < 9; i++) {
        QString temp = ins[i];
        temp = "\\b" + temp + "\\b";
        instructionPatterns.append(temp);
    }
    foreach (const QString &pattern, instructionPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = instructionFormat;
        highlightingRules.append(rule);
    }

    regFormat.setForeground(Qt::red);
    regFormat.setFontWeight(QFont::Bold);

    QStringList regPatterns;
    QString regname[32]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"};
    for(int i = 0; i < 32; i++) {
        regname[i] = "\\$" + regname[i];
        regPatterns.append(regname[i]);
    }
    foreach (const QString &pattern, regPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = regFormat;
            highlightingRules.append(rule);
    }

    multiLineCommentFormat.setForeground(Qt::red);
    labelFormat.setFontWeight(QFont::Bold);
    labelFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("^[A-Za-z]+:");
    rule.format = labelFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("#[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}


void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}


