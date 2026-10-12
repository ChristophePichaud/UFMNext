#pragma once

#include "ufmnext/gui/DrawingCanvas.h"
#include <QWidget>
#include <QScrollArea>

namespace ufmnext::gui {

class MdiChild : public QWidget {
    Q_OBJECT
public:
    explicit MdiChild(QWidget* parent = nullptr);
    ~MdiChild() override = default;

    void NewFile();
    bool LoadFile(const QString& filePath);
    bool SaveFile(const QString& filePath);
    bool ExportPng(const QString& filePath);
    void PrintPreview();

    QString CurrentFilePath() const noexcept { return m_currentFilePath; }
    QString UserFriendlyCurrentFile() const;
    bool IsModified() const;

    DrawingCanvas* GetCanvas() const noexcept { return m_canvas; }

signals:
    void documentModified();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void setupUI();
    void setCurrentFile(const QString& filePath);
    bool maybeSave();

    QScrollArea* m_scrollArea{nullptr};
    DrawingCanvas* m_canvas{nullptr};
    QString m_currentFilePath;
    bool m_isUntitled{true};
    static int s_sequenceNumber;
};

} // namespace ufmnext::gui
