#include "TaskExtension.h"

#include <QMenu>
#include <QRegularExpression>

#include "LinkSyntaxHighlighter.h"

static const char INSERT_TASK_REGEX[] = "^(\\s*)([-\\*])?(\\s)?(\\[[x ]\\])?";

static int getTaskCheckmarkPosUnderCursor(const QTextCursor& cursor) {
    return LinkSyntaxHighlighter::getTaskCheckmarkPosAt(cursor.block().text(),
                                                        cursor.positionInBlock());
}

static void toggleTask(QTextCursor* cursor, int pos) {
    // First insert the new character after the old one, then delete the old character.
    // This prevents the main cursor from moving as a side effect from the editing.
    cursor->beginEditBlock();
    cursor->setPosition(pos + cursor->block().position());
    cursor->movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    QString c = cursor->selectedText();
    cursor->movePosition(QTextCursor::Right);
    if (c == "x") {
        cursor->insertText(" ");
    } else {
        cursor->insertText("x");
    }
    cursor->movePosition(QTextCursor::Left);
    cursor->deletePreviousChar();
    cursor->endEditBlock();
}

TaskExtension::TaskExtension(TextEdit* textEdit)
        : TextEditExtension(textEdit), mInsertTaskAction(std::make_unique<QAction>()) {
    mInsertTaskAction->setText(tr("Insert task"));
    mInsertTaskAction->setShortcut(Qt::ALT | Qt::Key_T);
    connect(mInsertTaskAction.get(), &QAction::triggered, this, &TaskExtension::insertTask);
    mTextEdit->addAction(mInsertTaskAction.get());
}

void TaskExtension::aboutToShowContextMenu(QMenu* menu, const QPoint& pos) {
    QTextCursor cursor = mTextEdit->cursorForPosition(pos);
    int checkmarkPos = getTaskCheckmarkPosUnderCursor(cursor);
    if (checkmarkPos == -1) {
        return;
    }
    menu->addAction(tr("Toggle task completion"), this, [this, pos, checkmarkPos] {
        // TODO: why can I not pass the cursor here?
        QTextCursor c = mTextEdit->cursorForPosition(pos);
        toggleTask(&c, checkmarkPos);
    });
}

void TaskExtension::aboutToShowEditContextMenu(QMenu* menu, const QPoint& /*pos*/) {
    menu->addAction(mInsertTaskAction.get());
}

bool TaskExtension::keyPress(QKeyEvent* event) {
    bool ctrlPressed = event->modifiers() == Qt::CTRL;
    bool enterPressed = event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return;
    if (ctrlPressed && enterPressed) {
        return toggleTaskUnderCursor();
    }
    return false;
}

bool TaskExtension::mouseRelease(QMouseEvent* event) {
    if (event->modifiers() == Qt::CTRL) {
        toggleTaskUnderCursor();
    }
    return false;
}

void TaskExtension::insertTask() {
    auto cursor = mTextEdit->textCursor();
    QString text = cursor.block().text();

    QRegularExpression expression(INSERT_TASK_REGEX);
    QRegularExpressionMatch match = expression.match(text);
    if (!match.hasMatch()) {
        // should not happen
        return;
    } else if (!match.captured(3).isEmpty() && !match.captured(4).isEmpty()) {
        // line already contains a task, no action
        return;
    } else if (match.captured(2).isEmpty()) {
        // not in list
        cursor.setPosition(cursor.block().position() + match.capturedEnd(1));
        cursor.insertText("- [ ] ");
    } else {
        // in list, only add checkbox
        bool trailingSpace = !match.captured(3).isEmpty();
        cursor.setPosition(cursor.block().position() + match.capturedEnd(2));
        cursor.insertText(trailingSpace ? " [ ]" : " [ ] ");
    }
}

bool TaskExtension::toggleTaskUnderCursor() {
    auto cursor = mTextEdit->textCursor();
    int pos = getTaskCheckmarkPosUnderCursor(cursor);
    if (pos == -1) {
        return false;
    }
    toggleTask(&cursor, pos);
    return true;
}
