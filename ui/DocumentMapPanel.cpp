#include "DocumentMapPanel.h"
#include "DocumentMapWidget.h"
#include "../core/EditorWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

namespace NotepadPlusPlus {

DocumentMapPanel::DocumentMapPanel(QWidget *parent)
    : QDockWidget(tr("Document Map"), parent)
    , m_mapWidget(new DocumentMapWidget(this))
{
    // Create main widget
    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(mainWidget);
    layout->setContentsMargins(5, 5, 5, 5);
    
    // Add title
    QLabel* titleLabel = new QLabel(tr("Document Map"), this);
    titleLabel->setStyleSheet("font-weight: bold; padding: 2px;");
    layout->addWidget(titleLabel);
    
    // Add the map widget
    layout->addWidget(m_mapWidget, 1);
    
    setWidget(mainWidget);
    
    // Configure dock widget
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    // Connect signals
    connect(m_mapWidget, &DocumentMapWidget::positionClicked,
            this, &DocumentMapPanel::onPositionClicked);
    connect(m_mapWidget, &DocumentMapWidget::scrollRequested,
            this, &DocumentMapPanel::onScrollRequested);
}

DocumentMapPanel::~DocumentMapPanel() = default;

void DocumentMapPanel::setEditor(EditorWidget* editor)
{
    m_mapWidget->setEditor(editor);
}

void DocumentMapPanel::onPositionClicked(int line)
{
    emit gotoLineRequested(line);
}

void DocumentMapPanel::onScrollRequested(int deltaY)
{
    // For now, convert scroll to line navigation
    // In a full implementation, this would scroll the editor smoothly
    if (deltaY > 0) {
        // Scroll down
        emit gotoLineRequested(-1); // Special value for relative movement
    } else if (deltaY < 0) {
        // Scroll up  
        emit gotoLineRequested(-2); // Special value for relative movement
    }
}

} // namespace NotepadPlusPlus