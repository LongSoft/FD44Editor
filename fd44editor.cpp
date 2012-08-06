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
    connect(ui->dtsTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableDtsMagicComboBox(int)));
    connect(ui->uuidEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButtons()));
    connect(ui->macEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButtons()));
    connect(ui->mbsnEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButtons()));

    // Inserting data to dtsMagicBytesComboBox
    ui->dtsMagicBytesComboBox->setItemData(0, QByteArray::fromRawData(DTS_LONG_MAGIC_V1, sizeof(DTS_LONG_MAGIC_V1)));
    ui->dtsMagicBytesComboBox->setItemData(1, QByteArray::fromRawData(DTS_LONG_MAGIC_V2, sizeof(DTS_LONG_MAGIC_V2)));
    ui->dtsMagicBytesComboBox->setItemData(2, QByteArray::fromRawData(DTS_LONG_MAGIC_V3, sizeof(DTS_LONG_MAGIC_V3)));
}

FD44Editor::~FD44Editor()
{
    delete ui;
}


void FD44Editor::openImageFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open BIOS image file"),".","BIOS image file (*.rom *.bin *.cap)");

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
    QString path = QFileDialog::getSaveFileName(this, tr("Save BIOS image file"),".","BIOS image file (*.rom *.bin *.cap)");

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
    data.dts_type = UnknownDts;
    data.mac_storage = UnknownStorage;
    data.fd44.dts_long_magic = QByteArray::fromRawData(DTS_LONG_MAGIC_V1, sizeof(DTS_LONG_MAGIC_V1));

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
    pos += BOOTEFI_MOTHERBOARD_NAME_LENGTH + BOOTEFI_BIOS_DATE_OFFSET;
    data.be.bios_date = bios.mid(pos, BOOTEFI_BIOS_DATE_LENGTH);

    // Detecting ME presence
    bool isFull = false;
    pos = bios.indexOf(QByteArray::fromRawData(ME_HEADER, sizeof(ME_HEADER)));
    if (pos != -1)
        isFull = true;

    // Detecting primary LAN MAC storage
    pos = bios.indexOf(QByteArray::fromRawData(GBE_HEADER, sizeof(GBE_HEADER)));
    if (pos != -1)
    {
        int pos2 = bios.lastIndexOf(QByteArray::fromRawData(GBE_HEADER, sizeof(GBE_HEADER)));
        if (pos != pos2 && bios.mid(pos - MAC_LENGTH, MAC_LENGTH) == QByteArray(GBE_MAC_STUB, sizeof(GBE_MAC_STUB)))
            pos = pos2;

        data.gbe.mac = bios.mid(pos - MAC_LENGTH, MAC_LENGTH);
        data.mac_storage = GbE;
    }

    // Searching for non-empty module
    pos = bios.indexOf(QByteArray::fromRawData(MODULE_HEADER, sizeof(MODULE_HEADER)));
    if (pos == -1)
    {
        lastError = tr("Module is not found");
        data.state = ParseError;
        return data;
    }

    bool isEmpty = true;
    QByteArray module, moduleBody, moduleVersion;
    while (isEmpty && pos != -1)
    {
        module = bios.mid(pos, MODULE_LENGTH);

        // Checking for BSA_ signature
        if(module.mid(MODULE_HEADER_BSA_OFFSET, sizeof(MODULE_HEADER_BSA)) != QByteArray::fromRawData(MODULE_HEADER_BSA, sizeof(MODULE_HEADER_BSA)))
        {
            pos = bios.indexOf(QByteArray::fromRawData(MODULE_HEADER, sizeof(MODULE_HEADER)), pos+1);
            continue;
        }

        // Determining version
        moduleVersion = module.mid(sizeof(MODULE_HEADER), sizeof(MODULE_VERSION_X6X));
        if (moduleVersion == QByteArray::fromRawData(MODULE_VERSION_X6X, sizeof(MODULE_VERSION_X6X)))
            data.fd44.version = x6x;
        else if (moduleVersion == QByteArray::fromRawData(MODULE_VERSION_X7X, sizeof(MODULE_VERSION_X7X)))
            data.fd44.version = x7x;
        else
        {
            lastError = tr("Module version is unknown");
            data.state = ParseError;
            return data;
        }

        pos += MODULE_HEADER_LENGTH;
        
        // Checking for empty module
        moduleBody = module.right(MODULE_LENGTH - MODULE_HEADER_LENGTH);
        if (moduleBody.count('\xFF') != moduleBody.size())
            isEmpty = false;
        else
            pos = bios.indexOf(QByteArray::fromRawData(MODULE_HEADER, sizeof(MODULE_HEADER)), pos);
    }

    if (isEmpty)
    {
        data.state = Empty;
        return data;
    }

    // Data format setup
    QByteArray macHeader, shortDtsHeader, longDtsHeader, uuidHeader, mbsnHeader;
    if (data.fd44.version == x6x)
    {
        macHeader = QByteArray::fromRawData(MAC_HEADER_X6X, sizeof(MAC_HEADER_X6X));
        shortDtsHeader = QByteArray::fromRawData(DTS_SHORT_HEADER_X6X, sizeof(DTS_SHORT_HEADER_X6X));
        longDtsHeader = QByteArray::fromRawData(DTS_LONG_HEADER_X6X, sizeof(DTS_LONG_HEADER_X6X));
        uuidHeader = QByteArray::fromRawData(UUID_HEADER_X6X, sizeof(UUID_HEADER_X6X));
        mbsnHeader = QByteArray::fromRawData(MBSN_HEADER_X6X, sizeof(MBSN_HEADER_X6X));
    }
    else if (data.fd44.version == x7x)
    {
        macHeader = QByteArray::fromRawData(MAC_HEADER_X7X, sizeof(MAC_HEADER_X7X));
        shortDtsHeader = QByteArray::fromRawData(DTS_SHORT_HEADER_X7X, sizeof(DTS_SHORT_HEADER_X7X));
        longDtsHeader = QByteArray::fromRawData(DTS_LONG_HEADER_X7X, sizeof(DTS_LONG_HEADER_X7X));
        uuidHeader = QByteArray::fromRawData(UUID_HEADER_X7X, sizeof(UUID_HEADER_X7X));
        mbsnHeader = QByteArray::fromRawData(MBSN_HEADER_X7X, sizeof(MBSN_HEADER_X7X));
    }

    // Searching for MAC block
    pos = moduleBody.lastIndexOf(macHeader);
    if (pos != -1)
    {
        data.mac_storage = MAC;
        data.fd44.mac = QByteArray::fromHex(moduleBody.mid(pos + macHeader.length(), MAC_ASCII_LENGTH));
    }
    // Searching for DTS block
    data.dts_type = None;
    // Short DTS
    pos = moduleBody.lastIndexOf(shortDtsHeader);
    if(pos != -1)
    {
        pos += shortDtsHeader.length();
        data.dts_type = Short;
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
    pos = moduleBody.lastIndexOf(longDtsHeader);
    if (pos != -1)
    {
        pos += longDtsHeader.length();
        data.dts_type = Long;
        data.fd44.dts_key = moduleBody.mid(pos, DTS_KEY_LENGTH);
        pos += DTS_KEY_LENGTH;
        
        if(moduleBody.mid(pos, sizeof(DTS_LONG_PART2)) != QByteArray::fromRawData(DTS_LONG_PART2, sizeof(DTS_LONG_PART2)))
        {
            lastError = tr("Part 2 of long DTS header is unknown");
            data.state = ParseError;
            return data;
        }
        pos += sizeof(DTS_LONG_PART2);

        data.fd44.dts_long_magic = moduleBody.mid(pos, sizeof(DTS_LONG_MAGIC_V1));
        if (data.fd44.dts_long_magic != QByteArray::fromRawData(DTS_LONG_MAGIC_V1, sizeof(DTS_LONG_MAGIC_V1))
         && data.fd44.dts_long_magic != QByteArray::fromRawData(DTS_LONG_MAGIC_V2, sizeof(DTS_LONG_MAGIC_V2))
         && data.fd44.dts_long_magic != QByteArray::fromRawData(DTS_LONG_MAGIC_V3, sizeof(DTS_LONG_MAGIC_V3)))
        {
            lastError = tr("Long DTS magic bytes are unknown");
            data.state = ParseError;
            return data;
        }
        pos += sizeof(DTS_LONG_MAGIC_V1);

        if(moduleBody.mid(pos, sizeof(DTS_LONG_PART3)) != QByteArray::fromRawData(DTS_LONG_PART3, sizeof(DTS_LONG_PART3)))
        {
            lastError = tr("Part 3 of long DTS header is unknown");
            data.state = ParseError;
            return data;
        }
        pos += sizeof(DTS_LONG_PART3);

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
        
        if(moduleBody.mid(pos, sizeof(DTS_LONG_PART4)) != QByteArray::fromRawData(DTS_LONG_PART4, sizeof(DTS_LONG_PART4)))
        {
            lastError = tr("Part 4 of long DTS header is unknown");
            data.state = ParseError;
            return data;
        }
    }

    // Searching for UUID block
    pos = moduleBody.lastIndexOf(uuidHeader);
    if (pos == -1)
    {
        lastError = tr("UUID not found");
        data.state = ParseError;
        return data;
    }
    pos += uuidHeader.length();
    data.fd44.uuid = moduleBody.mid(pos, UUID_LENGTH);

    // If MAC is not found earlier, using MAC part of UUID
    if(data.mac_storage == UnknownStorage)
    {
        data.gbe.mac = data.fd44.uuid.right(MAC_LENGTH);
        data.fd44.mac = data.fd44.uuid.right(MAC_LENGTH);
        if(isFull)
            data.mac_storage = UUID;
    }

    // Searching for MBSN block
    pos = moduleBody.lastIndexOf(mbsnHeader);
    if (pos == -1)
    {
        lastError = tr("MBSN not found");
        data.state = ParseError;
        return data;
    }
    pos += mbsnHeader.length();
    data.fd44.mbsn = moduleBody.mid(pos, MBSN_BODY_LENGTH);

    data.state = Valid;
    return data;
}
QByteArray FD44Editor::writeToBIOS(const QByteArray & bios, const bios_t & data)
{
    int pos = bios.lastIndexOf(QByteArray::fromRawData(MODULE_HEADER, sizeof(MODULE_HEADER)));
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

    // Data format setup
    QByteArray macHeader, shortDtsHeader, longDtsHeader, uuidHeader, mbsnHeader;
    if (data.fd44.version == x6x)
    {
        macHeader = QByteArray::fromRawData(MAC_HEADER_X6X, sizeof(MAC_HEADER_X6X));
        shortDtsHeader = QByteArray::fromRawData(DTS_SHORT_HEADER_X6X, sizeof(DTS_SHORT_HEADER_X6X));
        longDtsHeader = QByteArray::fromRawData(DTS_LONG_HEADER_X6X, sizeof(DTS_LONG_HEADER_X6X));
        uuidHeader = QByteArray::fromRawData(UUID_HEADER_X6X, sizeof(UUID_HEADER_X6X));
        mbsnHeader = QByteArray::fromRawData(MBSN_HEADER_X6X, sizeof(MBSN_HEADER_X6X));
    }
    else if (data.fd44.version == x7x)
    {
        macHeader = QByteArray::fromRawData(MAC_HEADER_X7X, sizeof(MAC_HEADER_X7X));
        shortDtsHeader = QByteArray::fromRawData(DTS_SHORT_HEADER_X7X, sizeof(DTS_SHORT_HEADER_X7X));
        longDtsHeader = QByteArray::fromRawData(DTS_LONG_HEADER_X7X, sizeof(DTS_LONG_HEADER_X7X));
        uuidHeader = QByteArray::fromRawData(UUID_HEADER_X7X, sizeof(UUID_HEADER_X7X));
        mbsnHeader = QByteArray::fromRawData(MBSN_HEADER_X7X, sizeof(MBSN_HEADER_X7X));
    }

    QByteArray module;
    // Realtek MAC
    if(data.mac_storage == MAC)
    {
        module.append(macHeader);
        QByteArray encodedMac = data.fd44.mac.toHex().toUpper();
        module.append(encodedMac);
        module.append('\x00');
    }

    // Short DTS key
    if(data.dts_type == Short)
    {
        module.append(shortDtsHeader);
        module.append(data.fd44.dts_key);
        module.append(DTS_SHORT_PART2, sizeof(DTS_SHORT_PART2));
    }

    // Long DTS key
    if(data.dts_type == Long)
    {
        module.append(longDtsHeader);
        module.append(data.fd44.dts_key);
        module.append(DTS_LONG_PART2, sizeof(DTS_LONG_PART2));
        module.append(data.fd44.dts_long_magic);
        module.append(DTS_LONG_PART3, sizeof(DTS_LONG_PART3));
        QByteArray reversedKey;
        for(unsigned int i = 0; i < DTS_KEY_LENGTH; i++)
            reversedKey.append(data.fd44.dts_key.at(DTS_KEY_LENGTH-1-i) ^ DTS_LONG_MASK[i]);
        module.append(reversedKey);
        module.append(DTS_LONG_PART4, sizeof(DTS_LONG_PART4));
    }

    // UUID
    module.append(uuidHeader);
    module.append(data.fd44.uuid);
    if (data.mac_storage == GbE)
        module.append(data.gbe.mac);
    else
        module.append(data.fd44.mac);
    
    // MBSN
    module.append(mbsnHeader);
    module.append(data.fd44.mbsn);
    module.append('\x00');

    // FFs
    module.append(QByteArray(MODULE_LENGTH - MODULE_HEADER_LENGTH - module.length(), '\xFF'));

    // Replacing all modules
    QByteArray newBios = bios;
    pos = bios.indexOf(QByteArray::fromRawData(MODULE_HEADER, sizeof(MODULE_HEADER)));
    while(pos != -1)
    {
        // Checking for BSA_ signature
        if(bios.mid(pos + MODULE_HEADER_BSA_OFFSET, sizeof(MODULE_HEADER_BSA)) != QByteArray::fromRawData(MODULE_HEADER_BSA, sizeof(MODULE_HEADER_BSA)))
        {
            pos = bios.indexOf(QByteArray::fromRawData(MODULE_HEADER, sizeof(MODULE_HEADER)), pos + MODULE_HEADER_LENGTH);
            continue;
        }       
		
		pos += MODULE_HEADER_LENGTH;
        newBios.replace(pos, module.length(), module);
        pos += module.length();
        pos = bios.indexOf(QByteArray::fromRawData(MODULE_HEADER, sizeof(MODULE_HEADER)), pos);
    }

    if(data.mac_storage != GbE)
        return newBios;

    // Replacing MACs
    pos = newBios.indexOf(QByteArray(GBE_HEADER, sizeof(GBE_HEADER)));
    int pos2 = newBios.lastIndexOf(QByteArray(GBE_HEADER, sizeof(GBE_HEADER)));
    if (pos == -1)
    {
        lastError = tr("GbE region not found in target file. \nPlease use full BIOS backup or factory BIOS File.");
        return QByteArray();
    }

    newBios.replace(pos - MAC_LENGTH, MAC_LENGTH, data.gbe.mac);
    newBios.replace(pos2 - MAC_LENGTH, MAC_LENGTH, data.gbe.mac);
    return newBios;
}


void FD44Editor::writeToUI(bios_t data)
{
    switch (data.state)
    {
    case ParseError:
        QMessageBox::critical(this, tr("Fatal error"), tr("Error parsing BIOS data.\n%1").arg(lastError));
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
    ui->dateEdit->setText(data.be.bios_date);

    // List-based detection
    // LAN type detection
    bool lanDetected = false;
    if(data.mac_storage == UnknownStorage)
        for(unsigned int i = 0; i < MB_FEATURE_LIST_LENGTH; i++)
            if (data.be.motherboard_name == QByteArray(MB_FEATURE_LIST[i].name, BOOTEFI_MOTHERBOARD_NAME_LENGTH))
            {
                data.mac_storage = MB_FEATURE_LIST[i].mac_storage;
                ui->lanEdit->setText(tr("Found in database"));
                lanDetected = true;
            }
    // DTS type detection
    bool dtsDetected = false;
    if(data.dts_type == UnknownDts)
        for(unsigned int i = 0; i < MB_FEATURE_LIST_LENGTH; i++)
            if (data.be.motherboard_name == QByteArray(MB_FEATURE_LIST[i].name, BOOTEFI_MOTHERBOARD_NAME_LENGTH))
            {
                data.dts_type = MB_FEATURE_LIST[i].dts_type;
                ui->dtsEdit->setText(tr("Found in database"));
                dtsDetected = true;
            }

    switch(data.mac_storage)
    {
    case UnknownStorage:
        if (!lanDetected)
            ui->lanEdit->setText(tr("Not detected"));
        ui->lanComboBox->setEnabled(true);
		ui->macEdit->setText(data.fd44.mac.toHex());
        break;
    case GbE:
        if (!lanDetected)
            ui->lanEdit->setText(tr("Detected from GbE region"));
        ui->lanComboBox->setEnabled(false);
        ui->lanComboBox->setCurrentIndex(0);
        ui->macEdit->setText(data.gbe.mac.toHex());
        break;
    case MAC:
        if (!lanDetected)
            ui->lanEdit->setText(tr("Detected from module"));
        ui->lanComboBox->setEnabled(false);
        ui->lanComboBox->setCurrentIndex(1);
        ui->macEdit->setText(data.fd44.mac.toHex());
        break;
    case UUID:
        if (!lanDetected)
            ui->lanEdit->setText(tr("Detected from module"));
        ui->lanComboBox->setEnabled(false);
        ui->lanComboBox->setCurrentIndex(2);
        ui->macEdit->setText(data.fd44.mac.toHex());
        break;
    }
    
    ui->uuidEdit->setText(data.fd44.uuid.toHex()); // UUID
    ui->mbsnEdit->setText(data.fd44.mbsn); // MBSN


    switch (data.dts_type)	// DTS
    {
    case UnknownDts:
        if (!dtsDetected)
            ui->dtsEdit->setText(tr("Not detected"));
        ui->dtsGroupBox->setEnabled(true);
        ui->dtsTypeComboBox->setEnabled(true);
        ui->dtsMagicBytesComboBox->setEnabled(true);
        ui->dtsKeyEdit->setEnabled(true);
        break;
    case None:
        if (!dtsDetected)
            ui->dtsEdit->setText(tr("Detected from module"));
        ui->dtsTypeComboBox->setCurrentIndex(0);
        ui->dtsGroupBox->setEnabled(false);
        ui->dtsKeyEdit->setText("");
        break;
    case Short:
        if (!dtsDetected)
            ui->dtsEdit->setText(tr("Detected from module"));
        ui->dtsTypeComboBox->setCurrentIndex(1);
        ui->dtsGroupBox->setEnabled(true);
        ui->dtsTypeComboBox->setEnabled(false);
        ui->dtsMagicBytesComboBox->setEnabled(false);
        ui->dtsKeyEdit->setEnabled(true);
        ui->dtsKeyEdit->setText(data.fd44.dts_key.toHex());
        break;
    case Long:
        if (!dtsDetected)
            ui->dtsEdit->setText(tr("Detected from module"));
        ui->dtsTypeComboBox->setCurrentIndex(2);
        ui->dtsMagicBytesComboBox->setCurrentIndex(ui->dtsMagicBytesComboBox->findData(data.fd44.dts_long_magic));
        ui->dtsGroupBox->setEnabled(true);
        ui->dtsTypeComboBox->setEnabled(false);
        ui->dtsMagicBytesComboBox->setEnabled(false);
        ui->dtsKeyEdit->setEnabled(true);
        ui->dtsKeyEdit->setText(data.fd44.dts_key.toHex());
        break;
    }
    opened = data;
}
bios_t FD44Editor::readFromUI()
{
    bios_t data;

    data = opened;

    if(data.dts_type == UnknownDts)
    {
        switch (ui->dtsTypeComboBox->currentIndex())
        {
        case 0:
            data.dts_type = None;
            break;
        case 1:
            data.dts_type = Short;
            break;
        case 2:
            data.dts_type = Long;
            data.fd44.dts_long_magic = ui->dtsMagicBytesComboBox->itemData(ui->dtsMagicBytesComboBox->currentIndex()).toByteArray();
            break;
        }
    }

    if(data.mac_storage == UnknownStorage)
    {
        switch (ui->lanComboBox->currentIndex())
        {
        case 0:
            data.mac_storage = GbE;
            break;
        case 1:
            data.mac_storage = MAC;
            break;
        case 2:
            data.mac_storage = UUID;
            break;
        }
    }
    data.gbe.mac = QByteArray::fromHex(ui->macEdit->text().toAscii());
    data.fd44.mac = QByteArray::fromHex(ui->macEdit->text().toAscii());
    data.fd44.uuid = QByteArray::fromHex(ui->uuidEdit->text().toAscii());
    data.fd44.dts_key = QByteArray::fromHex(ui->dtsKeyEdit->text().toAscii());
    data.fd44.mbsn = ui->mbsnEdit->text().toAscii();

    return data;
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

void FD44Editor::enableDtsMagicComboBox(int index)
{
    ui->dtsMagicBytesComboBox->setEnabled(index == 2);
}
