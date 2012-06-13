#ifndef FD44EDITOR_H
#define FD44EDITOR_H

#include <QMainWindow>
#include <QByteArray>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "bios.h"

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
	void setMbsnInputMask(bool oldMbsn);
	void enableSaveButtons();
    void enableMac2Edit();

private:
    Ui::FD44Editor *ui;
	QString lastError;
    bios_t opened;

    bios_t readFromBIOS(const QByteArray & bios);
    QByteArray writeToBIOS(const QByteArray & bios, const bios_t & data);

    bios_t readFromUI();
    void writeToUI(bios_t &data);
};

#endif // FD44EDITOR_H
