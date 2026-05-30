#pragma once
#include <QLabel>

namespace NotepadPlusPlus {

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = nullptr);
    explicit ClickableLabel(const QString& text, QWidget* parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
};

} // namespace NotepadPlusPlus
