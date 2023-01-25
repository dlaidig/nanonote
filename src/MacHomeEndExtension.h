#ifndef MACHOMEENDEXTENSION_H
#define MACHOMEENDEXTENSION_H

#include "TextEdit.h"

class MacHomeEndExtension : public TextEditExtension {
    Q_OBJECT
public:
    explicit MacHomeEndExtension(TextEdit* textEdit);

    bool keyPress(QKeyEvent* event) override;
};

#endif // MACHOMEENDEXTENSION_H
