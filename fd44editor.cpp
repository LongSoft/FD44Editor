#include "fd44editor.h"
#include "ui_fd44editor.h"

FD44Editor::FD44Editor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FD44Editor)
{
    ui->setupUi(this);

    // Signal-slot connections
    connect(ui->fromFileButton, SIGNAL(clicked()), this, SLOT(openImageFile()));
    connect(ui->toFileButton, SIGNAL(clicked()), this, SLOT(saveImageFile()));
    connect(ui->oldMbsnFormatRadioButton, SIGNAL(toggled(bool)), this, SLOT(setMbsnInputMask(bool)));
    connect(ui->uuidEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButtons()));
    connect(ui->macEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButtons()));
    connect(ui->mbsnEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButtons()));
}

FD44Editor::~FD44Editor()
{
    delete ui;
}


void FD44Editor::openImageFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open BIOS image file"),".","BIOS image file (*.rom *.bin)");

    QFileInfo fileInfo = QFileInfo(path);
    if(!fileInfo.exists())
    {
        ui->statusBar->showMessage(tr("Please select existing BIOS image file"));
        return;
    }
    QFile inputFile;
    inputFile.setFileName(path);
    
    if(!inputFile.open(QFile::ReadOnly))
    {
        ui->statusBar->showMessage(tr("Can't open file for reading. Check file permissions"));
        return;
    }
    
    QByteArray biosImage = inputFile.readAll();
    inputFile.close();

    writeToUI(readFromBIOS(biosImage));
	
    ui->statusBar->showMessage(tr("Loaded: %1").arg(fileInfo.fileName()));
}

void FD44Editor::saveImageFile()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save BIOS image file"),".","BIOS image file (*.rom *.bin)");

    QFileInfo fileInfo = QFileInfo(path);
    if(!fileInfo.exists())
    {
        ui->statusBar->showMessage(tr("Please select existing BIOS image file"));
        return;
    }

    QFile outputFile;
    outputFile.setFileName(path);
    if(!outputFile.open(QFile::ReadWrite))
    {
        ui->statusBar->showMessage(tr("Can't open file for writing. Check file permissions"));
        return;
    }

    QByteArray bios = outputFile.readAll();

    QByteArray newBios = writeToBIOS(bios, readFromUI());
    if(newBios.isEmpty())
    {
        QMessageBox::critical(this, tr("Fatal error"), tr("Error parsing output file BIOS data.\n%1.").arg(lastError));
        return;
    }

    outputFile.seek(0);
    outputFile.write(newBios);
    outputFile.close();

    ui->statusBar->showMessage(tr("Written: %1").arg(fileInfo.fileName()));
}


bios_t FD44Editor::readFromBIOS(const QByteArray & bios)
{
    bios_t data;
    data.fd44.dts_type = UnknownDts;
    data.gbe.lan_type = UnknownLan;
    // Detecting motherboard model and BIOS version
    int pos = bios.lastIndexOf(QByteArray::fromRawData(BOOTEFI_HEADER, sizeof(BOOTEFI_HEADER)));
    if (pos == -1)
    {
        lastError = tr("$BOOTEFI$ is not found");
        data.state = ParseError;
        return data;
    }

    pos += sizeof(BOOTEFI_HEADER) + BOOTEFI_MAGIC_LENGTH;
    data.be.bios_version = bios.mid(pos, BOOTEFI_BIOS_VERSION_LENGTH);
    pos += BOOTEFI_BIOS_VERSION_LENGTH;
    data.be.motherboard_name = bios.mid(pos, BOOTEFI_MOTHERBOARD_NAME_LENGTH);

    // Detecting primary LAN card type
    pos = bios.indexOf(QByteArray::fromRawData(GBE_HEADER, sizeof(GBE_HEADER)));
    if (pos != -1) // Intel LAN
    {
        data.gbe.mac = bios.mid(pos - MAC_LENGTH, MAC_LENGTH);
        data.gbe.lan_type = Intel;
    }

    // Searching for non-empty module
    pos = bios.indexOf(QByteArray::fromRawData(MODULE_HEADER_PART1, sizeof(MODULE_HEADER_PART1)));
    if (pos == -1)
    {
        lastError = tr("Module is not found");
        data.state = ParseError;
        return data;
    }

    bool isEmpty = true;
    QByteArray module, moduleBody;
    while (isEmpty && pos != -1)
    {
        module = bios.mid(pos, MODULE_LENGTH);
        pos += sizeof(MODULE_HEADER_PART1) + MODULE_HEADER_ME_VERSION_LENGTH;

        // Checking 2nd part of module header
        if (module.mid(sizeof(MODULE_HEADER_PART1) + MODULE_HEADER_ME_VERSION_LENGTH, sizeof(MODULE_HEADER_PART2)) != QByteArray::fromRawData(MODULE_HEADER_PART2, sizeof(MODULE_HEADER_PART2)))
        {
            lastError = tr("Part 2 of module header is unknown");
            data.state = ParseError;
            return data;
        }

        // Checking for empty module
        moduleBody = module.right(MODULE_LENGTH - sizeof(MODULE_HEADER_PART1) - MODULE_HEADER_ME_VERSION_LENGTH - sizeof(MODULE_HEADER_PART2));
        if (moduleBody.count('\xFF') != moduleBody.size())
            isEmpty = false;
        else
            pos = bios.indexOf(QByteArray::fromRawData(MODULE_HEADER_PART1, sizeof(MODULE_HEADER_PART1)), pos);
    }

    if (isEmpty)
    {
        data.state = Empty;
        data.gbe.lan_type = UnknownLan;
        data.fd44.dts_type = UnknownDts;
        return data;
    }


    // Reading module data
    // Searching for MAC block
    pos = moduleBody.lastIndexOf(QByteArray::fromRawData(MAC_HEADER, sizeof(MAC_HEADER)));
    if (pos != -1)
    {
        data.gbe.lan_type = Realtek;
        data.fd44.mac = QByteArray::fromHex(moduleBody.mid(pos + sizeof(MAC_HEADER), MAC_ASCII_LENGTH));
    }
    // Searching for DTS block
    data.fd44.dts_type = None;
    // Short DTS
    pos = moduleBody.lastIndexOf(QByteArray::fromRawData(DTS_SHORT_HEADER, sizeof(DTS_SHORT_HEADER)));
    if(pos != -1)
    {
        pos += sizeof(DTS_SHORT_HEADER);
        data.fd44.dts_type = Short;
        data.fd44.dts_key = moduleBody.mid(pos, DTS_KEY_LENGTH);
        pos += DTS_KEY_LENGTH;

        if(moduleBody.mid(pos, sizeof(DTS_SHORT_PART2)) != QByteArray::fromRawData(DTS_SHORT_PART2, sizeof(DTS_SHORT_PART2)))
        {
            lastError = tr("Part 2 of short DTS header is unknown");
            data.state = ParseError;
            return data;
        }
    }
    // Long DTS
    pos = moduleBody.lastIndexOf(QByteArray::fromRawData(DTS_LONG_HEADER, sizeof(DTS_LONG_HEADER)));
    if (pos != -1)
    {
        pos += sizeof(DTS_LONG_HEADER);
        data.fd44.dts_type = Long;
        data.fd44.dts_key = moduleBody.mid(pos, DTS_KEY_LENGTH);
        pos += DTS_KEY_LENGTH;
		
        if(moduleBody.mid(pos, sizeof(DTS_LONG_PART2)) != QByteArray::fromRawData(DTS_LONG_PART2, sizeof(DTS_LONG_PART2)))
        {
            lastError = tr("Part 2 of long DTS header is unknown");
            data.state = ParseError;
            return data;
        }
        pos += sizeof(DTS_LONG_PART2);

        QByteArray reversedKey = moduleBody.mid(pos, DTS_KEY_LENGTH);
        bool reversed = true;
        for(unsigned int i = 0; i < DTS_KEY_LENGTH; i++)
        {
            reversed = reversed && (data.fd44.dts_key.at(i) == (reversedKey.at(DTS_KEY_LENGTH-1-i) ^ DTS_LONG_MASK[i]));
        }
        if(!reversed)
        {
            lastError = tr("Second key bytes in long DTS are not reversed first key bytes");
            data.state = ParseError;
            return data;
        }
        pos += DTS_KEY_LENGTH;
		
        if(moduleBody.mid(pos, sizeof(DTS_LONG_PART3)) != QByteArray::fromRawData(DTS_LONG_PART3, sizeof(DTS_LONG_PART3)))
        {
            lastError = tr("Part 3 of long DTS header is unknown");
            data.state = ParseError;
            return data;
        }
    }

    // Searching for UUID block
    pos = moduleBody.lastIndexOf(QByteArray::fromRawData(UUID_HEADER, sizeof(UUID_HEADER)));
    if (pos == -1)
    {
        lastError = tr("UUID not found");
        data.state = ParseError;
        return data;
    }
    pos += sizeof(UUID_HEADER);
    data.fd44.uuid = moduleBody.mid(pos, UUID_LENGTH);

    // If MAC is not found earlier, using MAC part of UUID
    if(data.gbe.lan_type == UnknownLan)
    {
        data.gbe.mac = data.fd44.uuid.right(MAC_LENGTH);
        data.fd44.mac = data.fd44.uuid.right(MAC_LENGTH);
    }

    // Searching for MBSN block
    pos = moduleBody.lastIndexOf(QByteArray::fromRawData(MBSN_HEADER, sizeof(MBSN_HEADER)));
    if (pos == -1)
    {
        lastError = tr("MBSN not found");
        data.state = ParseError;
        return data;
    }
    pos += sizeof(MBSN_HEADER);
    data.fd44.mbsn = moduleBody.mid(pos, MBSN_BODY_LENGTH);

    data.state = Valid;
    return data;
}
QByteArray FD44Editor::writeToBIOS(const QByteArray & bios, const bios_t & data)
{
    int pos = bios.lastIndexOf(QByteArray::fromRawData(MODULE_HEADER_PART1, sizeof(MODULE_HEADER_PART1)));
    if (pos == -1)
    {
        lastError = tr("Module is not found");
        return QByteArray();
    }

    pos = bios.lastIndexOf(QByteArray::fromRawData(BOOTEFI_HEADER, sizeof(BOOTEFI_HEADER)));
    if (pos == -1)
    {
        lastError = tr("$BOOTEFI$ is not found");
        return QByteArray();
    }

    pos += sizeof(BOOTEFI_HEADER) + BOOTEFI_MAGIC_LENGTH + BOOTEFI_BIOS_VERSION_LENGTH;
    QByteArray motherboard_name = bios.mid(pos, BOOTEFI_MOTHERBOARD_NAME_LENGTH);   
	
    if (data.be.motherboard_name != motherboard_name)
    {
        lastError = tr("Motherboard model of loaded data are different from motherboard model in selected file.\n"\
					   "Loaded motherboard: %1\n"\
					   "Motherboard in selected file: %2")
					   .arg(QString(data.be.motherboard_name))
					   .arg(QString(motherboard_name));
        return QByteArray();
    }

    QByteArray module;
    // Realtek MAC
    if(data.gbe.lan_type == Realtek)
    {
        module.append(MAC_HEADER, sizeof(MAC_HEADER));
        QByteArray encodedMac = data.fd44.mac.toHex().toUpper();
        module.append(encodedMac);
        module.append('\x00');
    }

    // Short DTS key
    if(data.fd44.dts_type == Short)
    {
        module.append(DTS_SHORT_HEADER, sizeof(DTS_SHORT_HEADER));
		module.append(data.fd44.dts_key);
        module.append(DTS_SHORT_PART2, sizeof(DTS_SHORT_PART2));
    }

    // Long DTS key
    if(data.fd44.dts_type == Long)
    {
        module.append(DTS_LONG_HEADER, sizeof(DTS_LONG_HEADER));
        module.append(data.fd44.dts_key);
        module.append(DTS_LONG_PART2, sizeof(DTS_LONG_PART2));
        QByteArray reversedKey;
        for(unsigned int i = 0; i < DTS_KEY_LENGTH; i++)
            reversedKey.append(data.fd44.dts_key.at(DTS_KEY_LENGTH-1-i) ^ DTS_LONG_MASK[i]);
        module.append(reversedKey);
        module.append(DTS_LONG_PART3, sizeof(DTS_LONG_PART3));
    }

    // UUID
    module.append(UUID_HEADER, sizeof(UUID_HEADER));
    module.append(data.fd44.uuid);
    if (data.gbe.lan_type == Realtek)
        module.append(data.fd44.mac);
    else
        module.append(data.gbe.mac);
	
    // MBSN
    module.append(MBSN_HEADER, sizeof(MBSN_HEADER));
    module.append(data.fd44.mbsn);
    module.append('\x00');

    // FFs
    module.append(QByteArray(MODULE_LENGTH - sizeof(MODULE_HEADER_PART1) - MODULE_HEADER_ME_VERSION_LENGTH - sizeof(MODULE_HEADER_PART2) - module.length(), '\xFF'));

    // Replacing all modules
    QByteArray newBios = bios;
    pos = bios.indexOf(QByteArray::fromRawData(MODULE_HEADER_PART1, sizeof(MODULE_HEADER_PART1)));
    while(pos != -1)
    {
        pos += sizeof(MODULE_HEADER_PART1) + MODULE_HEADER_ME_VERSION_LENGTH + sizeof(MODULE_HEADER_PART2);
		newBios.replace(pos, module.length(), module);
        pos += module.length();
        pos = bios.indexOf(QByteArray::fromRawData(MODULE_HEADER_PART1, sizeof(MODULE_HEADER_PART1)), pos);
    }

    if(data.gbe.lan_type == Realtek)
        return newBios;

    // Replacing MACs
    pos = newBios.indexOf(QByteArray(GBE_HEADER, sizeof(GBE_HEADER)));
    int pos2 = newBios.lastIndexOf(QByteArray(GBE_HEADER, sizeof(GBE_HEADER)));
    if (pos != -1)
    {
        newBios.replace(pos - MAC_LENGTH, MAC_LENGTH, data.gbe.mac);
        newBios.replace(pos2 - MAC_LENGTH, MAC_LENGTH, data.gbe.mac);
    }
    return newBios;
}


void FD44Editor::writeToUI(bios_t data)
{
    switch (data.state)
    {
    case ParseError:
        QMessageBox::critical(this, tr("Fatal error"), tr("Error parsing BIOS data.\n%1.").arg(lastError));
        return;
    case Empty:
        QMessageBox::information(this, tr("Loaded module is empty"), tr("Loaded module is empty.\nIt is normal, if you are opening BIOS file downloaded from asus.com\n"\
                                                                        "If you are loading module from system BIOS, you must restore the module in your BIOS.\n"));
        break;
    case Valid:
        break;
    }

    ui->uuidGroupBox->setEnabled(true);
    ui->mbsnGroupBox->setEnabled(true);
    ui->macGroupBox->setEnabled(true);

    ui->mbEdit->setText(data.be.motherboard_name);
    ui->versionEdit->setText(QString("%1%2").arg((int)data.be.bios_version.at(0),2,10,QChar('0')).arg((int)data.be.bios_version.at(1),2,10,QChar('0')));

    // List-based detection
    // LAN type detection
    bool lanDetected = false;
    if(data.gbe.lan_type == UnknownLan)
        for(unsigned int i = 0; i < MB_FEATURE_LIST_LENGTH; i++)
            if (data.be.motherboard_name == QByteArray(MB_FEATURE_LIST[i].name, BOOTEFI_MOTHERBOARD_NAME_LENGTH))
            {
                data.gbe.lan_type = MB_FEATURE_LIST[i].lan_type;
                ui->lanEdit->setText(tr("Found in database"));
                lanDetected = true;
            }
    // DTS type detection
    bool dtsDetected = false;
    if(data.fd44.dts_type == UnknownDts)
        for(unsigned int i = 0; i < MB_FEATURE_LIST_LENGTH; i++)
            if (data.be.motherboard_name == QByteArray(MB_FEATURE_LIST[i].name, BOOTEFI_MOTHERBOARD_NAME_LENGTH))
            {
                data.fd44.dts_type = MB_FEATURE_LIST[i].dts_type;
                ui->dtsEdit->setText(tr("Found in database"));
                dtsDetected = true;
            }

    switch(data.gbe.lan_type)
    {
    case UnknownLan:
        if (!lanDetected)
            ui->lanEdit->setText(tr("Not detected"));
		ui->lanComboBox->setEnabled(true);
        break;
    case Realtek:
        if (!lanDetected)
            ui->lanEdit->setText(tr("Detected from module"));
		ui->lanComboBox->setEnabled(false);
		ui->lanComboBox->setCurrentIndex(0);
        ui->macEdit->setText(data.fd44.mac.toHex());
        break;
    case Intel:
        if (!lanDetected)
            ui->lanEdit->setText(tr("Detected from GbE region"));
		ui->lanComboBox->setEnabled(false);
		ui->lanComboBox->setCurrentIndex(1);
        ui->macEdit->setText(data.gbe.mac.toHex());
        break;
    /*case DualIntel:
        if (!lanDetected)
            ui->lanEdit->setText(tr("Detected from GbE region"));
        ui->lanComboBox->setEnabled(false);
        ui->lanComboBox->setCurrentIndex(2);
        ui->macEdit->setText(data.gbe.mac.toHex());
        ui->mac2Edit->setText(data.gbe.mac2.toHex());*/
    }
	
    ui->uuidEdit->setText(data.fd44.uuid.toHex()); // UUID

    if(data.fd44.mbsn.left(sizeof(MBSN_OLD_FORMAT_SIGN)) == QByteArray(MBSN_OLD_FORMAT_SIGN,sizeof(MBSN_OLD_FORMAT_SIGN)))
        ui->oldMbsnFormatRadioButton->setChecked(true);
    else
        ui->newMbsnFormatRadioButton->setChecked(true);

    ui->mbsnEdit->setText(data.fd44.mbsn); // MBSN


    switch (data.fd44.dts_type)	// DTS
	{
    case UnknownDts:
        if (!dtsDetected)
            ui->dtsEdit->setText(tr("Not detected"));
        ui->dtsGroupBox->setEnabled(true);
        break;
    case None:
        if (!dtsDetected)
            ui->dtsEdit->setText(tr("Detected from module"));
        ui->noDtsRadioButton->setChecked(true);
        ui->dtsGroupBox->setEnabled(false);
		break;
	case Short:
        if (!dtsDetected)
            ui->dtsEdit->setText(tr("Detected from module"));
        ui->shortDtsRadioButton->setChecked(true);
        ui->dtsGroupBox->setEnabled(false);
		break;
	case Long:
        if (!dtsDetected)
            ui->dtsEdit->setText(tr("Detected from module"));
        ui->longDtsRadioButton->setChecked(true);
        ui->dtsGroupBox->setEnabled(false);
		break;
	}
    opened = data;
}
bios_t FD44Editor::readFromUI()
{
    bios_t data;

    data = opened;

    if(data.fd44.dts_type == UnknownDts)
    {
        if(ui->noDtsRadioButton->isChecked())
            data.fd44.dts_type = None;
        if(ui->shortDtsRadioButton->isChecked())
            data.fd44.dts_type = Short;
        if(ui->longDtsRadioButton->isChecked())
            data.fd44.dts_type = Long;
    }

    if(data.gbe.lan_type == UnknownLan)
    {
        if(ui->lanComboBox->currentIndex() == 1)
            data.gbe.lan_type = Intel;
        /*else if (ui->lanComboBox->currentIndex() == 2)
            data.gbe.lan_type = DualIntel;*/
        else
            data.gbe.lan_type = Realtek;
    }
    data.gbe.mac = QByteArray::fromHex(ui->macEdit->text().toAscii());
    data.fd44.mac = QByteArray::fromHex(ui->macEdit->text().toAscii());
    data.fd44.uuid = QByteArray::fromHex(ui->uuidEdit->text().toAscii());
    data.fd44.dts_key = data.fd44.uuid.right(DTS_KEY_LENGTH);
    data.fd44.mbsn = ui->mbsnEdit->text().toAscii();

    return data;
}


void FD44Editor::setMbsnInputMask(bool old)
{
    if(old)
    {
        newMbsn = ui->mbsnEdit->text();
        ui->mbsnEdit->setInputMask(">MT7NNNNNNNNNNNN;_");
        ui->mbsnEdit->setText(oldMbsn);
    }
    else
    {
        oldMbsn = ui->mbsnEdit->text();
        ui->mbsnEdit->setInputMask("999999999999999;_");
        ui->mbsnEdit->setText(newMbsn);
    }
}

void FD44Editor::enableSaveButtons()
{
    if (ui->uuidEdit->text().length() == ui->uuidEdit->maxLength()
        && ui->macEdit->text().length() == ui->macEdit->maxLength()
        && ui->mbsnEdit->text().length() == ui->mbsnEdit->maxLength())
            ui->toFileButton->setEnabled(true);
    else
        ui->toFileButton->setEnabled(false);
}
