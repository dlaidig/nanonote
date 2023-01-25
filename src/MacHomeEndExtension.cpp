#include "MacHomeEndExtension.h"

#include <QAction>
#include <QDebug>
#include <QMenu>
#include <QTextBlock>

MacHomeEndExtension::MacHomeEndExtension(TextEdit* textEdit) : TextEditExtension(textEdit) {
}

bool MacHomeEndExtension::keyPress(QKeyEvent* event) {
    if (event->key() == Qt::Key_Home) {
        auto cursor = mTextEdit->textCursor();
        if (event->modifiers().testFlag(Qt::ShiftModifier)) {
            cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
        } else {
            cursor.movePosition(QTextCursor::StartOfLine);
        }
        mTextEdit->setTextCursor(cursor);
        return true;
    }
    if (event->key() == Qt::Key_End) {
        auto cursor = mTextEdit->textCursor();
        if (event->modifiers().testFlag(Qt::ShiftModifier)) {
            cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        } else {
            cursor.movePosition(QTextCursor::EndOfLine);
        }
        mTextEdit->setTextCursor(cursor);
        return true;
    }
    return false;
}
