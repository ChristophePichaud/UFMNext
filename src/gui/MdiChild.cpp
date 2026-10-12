#include "ufmnext/gui/MdiChild.h"
#include "ufmnext/core/Serializer.h"

#include <QVBoxLayout>
#include <QFileInfo>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>

namespace ufmnext::gui {

int MdiChild::s_sequenceNumber = 1;

MdiChild::MdiChild(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void MdiChild::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(false);
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setStyleSheet("QScrollArea { background: #505050; border: none; }");

    m_canvas = new DrawingCanvas(std::make_shared<core::Document>(), m_scrollArea);
    m_scrollArea->setWidget(m_canvas);

    layout->addWidget(m_scrollArea);

    connect(m_canvas, &DrawingCanvas::documentModified, this, [this]() {
        setWindowModified(m_canvas->GetDocument()->IsModified());
        emit documentModified();
    });
}

void MdiChild::NewFile() {
    m_isUntitled = true;
    m_currentFilePath = tr("Drawing %1.ufm").arg(s_sequenceNumber++);
    setWindowTitle(m_currentFilePath + "[*]");
}

bool MdiChild::LoadFile(const QString& filePath) {
    auto doc = m_canvas->GetDocument();
    if (!core::Serializer::LoadFromFile(filePath.toStdString(), *doc)) {
        return false;
    }
    setCurrentFile(filePath);
    m_canvas->update();
    return true;
}

bool MdiChild::SaveFile(const QString& filePath) {
    auto doc = m_canvas->GetDocument();
    if (!core::Serializer::SaveToFile(*doc, filePath.toStdString())) {
        return false;
    }
    setCurrentFile(filePath);
    doc->SetModified(false);
    setWindowModified(false);
    return true;
}

bool MdiChild::ExportPng(const QString& filePath) {
    return m_canvas->ExportToPng(filePath);
}

void MdiChild::PrintPreview() {
    m_canvas->ShowPrintPreview();
}

void MdiChild::setCurrentFile(const QString& filePath) {
    m_currentFilePath = QFileInfo(filePath).canonicalFilePath();
    m_isUntitled = false;
    m_canvas->GetDocument()->SetModified(false);
    setWindowModified(false);
    setWindowTitle(UserFriendlyCurrentFile() + "[*]");
}

QString MdiChild::UserFriendlyCurrentFile() const {
    return QFileInfo(m_currentFilePath).fileName();
}

bool MdiChild::IsModified() const {
    return m_canvas && m_canvas->GetDocument() && m_canvas->GetDocument()->IsModified();
}

bool MdiChild::maybeSave() {
    if (!IsModified()) {
        return true;
    }

    const auto ret = QMessageBox::warning(this, tr("UFMNext Drawing"),
                                          tr("'%1' has been modified.\nDo you want to save your changes?")
                                              .arg(UserFriendlyCurrentFile()),
                                          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch (ret) {
        case QMessageBox::Save: {
            QString path = m_currentFilePath;
            if (m_isUntitled) {
                path = QFileDialog::getSaveFileName(this, tr("Save Drawing Raw Data"),
                                                    m_currentFilePath,
                                                    tr("UFM Raw Data (*.ufm);;All Files (*)"));
                if (path.isEmpty()) return false;
            }
            return SaveFile(path);
        }
        case QMessageBox::Cancel:
            return false;
        default:
            break;
    }
    return true;
}

void MdiChild::closeEvent(QCloseEvent* event) {
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

} // namespace ufmnext::gui
