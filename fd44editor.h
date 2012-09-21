#ifndef FD44EDITOR_H
#define FD44EDITOR_H

#include <QMainWindow>
#include <QByteArray>
#include <QClipboard>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "motherboards.h"

namespace Ui {
class FD44Editor;
}

class FD44Editor : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit FD44Editor(QWidget *parent = 0);
    ~FD44Editor();

private slots:
    void openImageFile();
    void saveImageFile();
    void copyToClipboard();
    void enableSaveButton();

private:
    Ui::FD44Editor *ui;
    QString lastError;
    bios_t opened;

    bios_t readFromBIOS(const QByteArray & data);
    QByteArray writeToBIOS(const QByteArray & data, const bios_t & bios);

    bios_t readFromUI();
    void writeToUI(bios_t bios);
};

#endif // FD44EDITOR_H
