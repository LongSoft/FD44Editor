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
    connect(ui->copyButton, SIGNAL(clicked()), this, SLOT(copyToClipboard()));
    connect(ui->uuidEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButton()));
    connect(ui->macEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButton()));
    connect(ui->mbsnEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButton()));
    connect(ui->dtsKeyEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButton()));

    // Enable Drag-and-Drop actions
    setAcceptDrops(true);
}

FD44Editor::~FD44Editor()
{
    delete ui;
}

void FD44Editor::openImageFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open BIOS image file"),".","BIOS image file (*.rom *.bin *.cap);;All files (*.*)");
    openImageFile(path);
}

void FD44Editor::openImageFile(QString path)
{
    QFileInfo fileInfo = QFileInfo(path);
    if(!fileInfo.exists())
    {
        ui->statusBar->showMessage(tr("Please select existing BIOS image file."));
        return;
    }

    QFile inputFile;
    inputFile.setFileName(path);

    if(!inputFile.open(QFile::ReadOnly))
    {
        ui->statusBar->showMessage(tr("Can't open file for reading. Check file permissions."));
        return;
    }

    QByteArray biosImage = inputFile.readAll();
    inputFile.close();

    if(writeToUI(readFromBIOS(biosImage)))
        ui->statusBar->showMessage(tr("Loaded: %1").arg(fileInfo.fileName()));
}

void FD44Editor::saveImageFile()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save BIOS image file"),".","BIOS image file (*.rom *.bin *.cap);;All files (*.*)");

    QFileInfo fileInfo = QFileInfo(path);
    if(!fileInfo.exists())
    {
        ui->statusBar->showMessage(tr("Please select existing BIOS image file."));
        return;
    }

    QFile outputFile;
    outputFile.setFileName(path);
    if(!outputFile.open(QFile::ReadWrite))
    {
        ui->statusBar->showMessage(tr("Can't open file for writing. Check file permissions."));
        return;
    }

    QByteArray bios = outputFile.readAll();

    if(bios.left(UBF_FILE_HEADER.length()) == UBF_FILE_HEADER)
    {
        bios = bios.mid(UBF_FILE_HEADER_SIZE); 
    }

    QByteArray newBios = writeToBIOS(bios, readFromUI());
    if(newBios.isEmpty())
    {
        QMessageBox::critical(this, tr("Fatal error"), tr("Error parsing output file.\n%1").arg(lastError));
        return;
    }
    
    outputFile.resize(bios.length());
    outputFile.seek(0);
    outputFile.write(newBios);
    outputFile.close();
    outputFile.rename(QString("%1/%2.bin").arg(fileInfo.path()).arg(fileInfo.completeBaseName()));

    ui->statusBar->showMessage(tr("Written: %1.bin").arg(fileInfo.completeBaseName()));
}

void FD44Editor::copyToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(tr("BIOS information:\n"\
                          "Motherboard name: %1\n"\
                          "BIOS date: %2\n"\
                          "BIOS version: %3\n"\
                          "ME version: %4\n"\
                          "GbE version: %5\n"\
                          "LAN status: %6\n"\
                          "DTS key status: %7\n\n"\
                          "Editable data:\n"\
                          "Primary LAN MAC: %8\n"\
                          "DTS key: %9\n"
                          "UUID: %10\n"\
                          "MBSN: %11")
                       .arg(ui->mbEdit->text())
                       .arg(ui->dateEdit->text())
                       .arg(ui->biosVersionEdit->text())
                       .arg(ui->meVersionEdit->text())
                       .arg(ui->gbeVersionEdit->text())
                       .arg(ui->lanEdit->text())
                       .arg(ui->dtsEdit->text())
                       .arg(ui->macEdit->text().remove(':'))
                       .arg(ui->dtsKeyEdit->text().remove(' ').isEmpty() ? tr("Not present") : ui->dtsKeyEdit->text().remove(' '))
                       .arg(ui->uuidEdit->text().remove(' ').append(ui->macEdit->text().remove(':')))
                       .arg(ui->mbsnEdit->text())
                       );
}

bios_t FD44Editor::readFromBIOS(const QByteArray & data)
{
    bios_t bios;
    
    // Detecting motherboard model and BIOS version
    int pos = data.lastIndexOf(BOOTEFI_HEADER);
    if (pos == -1)
    {
        lastError = tr("$BOOTEFI$ signature not found.\nPlease open correct ASUS BIOS file.");
        bios.data.state = ParseError;
        return bios;
    }

    pos += BOOTEFI_HEADER.length() + BOOTEFI_MAGIC_LENGTH;
    bios.data.be.bios_version = data.mid(pos, BOOTEFI_BIOS_VERSION_LENGTH);
    pos += BOOTEFI_BIOS_VERSION_LENGTH;
    bios.data.be.motherboard_name = data.mid(pos, BOOTEFI_MOTHERBOARD_NAME_LENGTH);
    pos += BOOTEFI_MOTHERBOARD_NAME_LENGTH + BOOTEFI_BIOS_DATE_OFFSET;
    bios.data.be.bios_date = data.mid(pos, BOOTEFI_BIOS_DATE_LENGTH);

    // Looking up detected motherboard in list of supported motherboards
    bool isSupported = false;
    for(unsigned int i = 0; i < SUPPORTED_MOTHERBOARDS_LIST_LENGTH; i++)
    {
        if(!strcmp(bios.data.be.motherboard_name.constData(), SUPPORTED_MOTHERBOARDS_LIST[i].name))
        {
            isSupported = true;
            bios.mb = SUPPORTED_MOTHERBOARDS_LIST[i];
        }
    }

    // TODO: add detection using ASUSBKP$ data
    if(!isSupported)
    {
        lastError = tr("Motherboard model %1 not supported.\nSend this BIOS file to the program author.").arg(bios.data.be.motherboard_name.constData());
        bios.data.state = ParseError;
        return bios;
    }

    // Detecting ME presence and version
    bool isFull = false;
    pos = data.indexOf(ME_HEADER);
    if (pos != -1)
    {
        pos = data.indexOf(ME_VERSION_HEADER, pos);
        if (pos != -1)
        {
            bios.data.me.me_version = data.mid(pos + ME_VERSION_HEADER.length() + ME_VERSION_OFFSET, ME_VERSION_LENGTH);
        }
        isFull = true;
    }

    // Detecting GbE presence and version
    bool hasGbE = false;
    if(bios.mb.mac_type == GbE)
    {
        pos = data.indexOf(GBE_HEADER);
        if (pos != -1)
        {
            int pos2 = data.lastIndexOf(GBE_HEADER);
            if (pos != pos2 && data.mid(pos + GBE_MAC_OFFSET - MAC_LENGTH, MAC_LENGTH) == GBE_MAC_STUB)
                pos = pos2;

            bios.data.gbe.mac = data.mid(pos + GBE_MAC_OFFSET - MAC_LENGTH, MAC_LENGTH);
            bios.data.gbe.gbe_version = data.mid(pos + GBE_VERSION_OFFSET, GBE_VERSION_LENGTH);
            hasGbE = true;
        }
    }

    // Searching for non-empty module
    pos = data.indexOf(MODULE_HEADER);
    if (pos == -1)
    {
        lastError = tr("FD44 module not found.");
        bios.data.state = ParseError;
        return bios;
    }

    bool isEmpty = true;
    unsigned int moduleLength;
    QByteArray module, moduleBody, moduleVersion;
    while (isEmpty && pos != -1)
    {
        // Checking for BSA_ signature
        if(data.mid(pos + MODULE_HEADER_BSA_OFFSET, MODULE_HEADER_BSA.length()) != MODULE_HEADER_BSA)
        {
            pos = data.indexOf(MODULE_HEADER, pos+1);
            continue;
        }
        
        // Reading module length
        moduleLength = (data.at(pos + MODULE_LENGTH_OFFSET + 2) << 16) +
                       (data.at(pos + MODULE_LENGTH_OFFSET + 1) << 8)  +
                        data.at(pos + MODULE_LENGTH_OFFSET);
        
        module = data.mid(pos, moduleLength);

        // Determining version
        moduleVersion = module.mid(MODULE_HEADER.length(), bios.mb.module_version.length());
        if (moduleVersion != bios.mb.module_version)
        {
            lastError = tr("FD44 module version is unknown.");
            bios.data.state = ParseError;
            return bios;
        }

        pos += MODULE_HEADER_LENGTH;
        
        // Checking for empty module
        moduleBody = module.right(moduleLength - MODULE_HEADER_LENGTH);
        if (moduleBody.count('\xFF') != moduleBody.size())
            isEmpty = false;
        else
            pos = data.indexOf(MODULE_HEADER, pos+1);
    }

    if (isEmpty)
    {
        bios.data.state = Empty;
        return bios;
    }

    // ASCII MAC address
    if (bios.mb.mac_type == ASCII)
    {
        pos = moduleBody.indexOf(bios.mb.mac_header);
        if(pos == -1)
        {
            lastError = tr("ASCII MAC address required but not found.");
            bios.data.state = ParseError;
            return bios;    
        }

        pos += bios.mb.mac_header.length();

        if(bios.mb.mac_header == ASCII_MAC_HEADER_7_SERIES)
        {
            bios.data.module.mac_magic = moduleBody.at(pos);
            pos += ASCII_MAC_OFFSET;
        }

        bios.data.module.mac = QByteArray::fromHex(moduleBody.mid(pos, ASCII_MAC_LENGTH));
    }
    
    // DTS key
    // Short DTS
    if(bios.mb.dts_type == Short)
    {
        pos = moduleBody.indexOf(bios.mb.dts_header);
        if(pos == -1)
        {
            lastError = tr("Short DTS key required but not found.");
            bios.data.state = ParseError;
            return bios;    
        }

        pos += bios.mb.dts_header.length();
        bios.data.module.dts_key = moduleBody.mid(pos, DTS_KEY_LENGTH);
        pos += DTS_KEY_LENGTH;

        if(moduleBody.mid(pos, DTS_SHORT_PART2.length()) != DTS_SHORT_PART2)
        {
            lastError = tr("Part 2 of short DTS key is unknown.");
            bios.data.state = ParseError;
            return bios;
        }
    }
    // Long DTS
    if(bios.mb.dts_type == Long)
    {
        pos = moduleBody.indexOf(bios.mb.dts_header);
        if(pos == -1)
        {
            lastError = tr("Long DTS key required but not found.");
            bios.data.state = ParseError;
            return bios;
        }

        pos += bios.mb.dts_header.length();
        bios.data.module.dts_key = moduleBody.mid(pos, DTS_KEY_LENGTH);
        pos += DTS_KEY_LENGTH;

        if(moduleBody.mid(pos, DTS_LONG_PART2.length()) !=DTS_LONG_PART2)
        {
            lastError = tr("Part 2 of long DTS key is unknown.");
            bios.data.state = ParseError;
            return bios;
        }
        pos += DTS_LONG_PART2.length();

        bios.data.module.dts_magic = moduleBody.mid(pos, DTS_LONG_MAGIC_LENGTH);
        pos += DTS_LONG_MAGIC_LENGTH;

        if(moduleBody.mid(pos, DTS_LONG_PART3.length()) != DTS_LONG_PART3)
        {
            lastError = tr("Part 3 of long DTS key is unknown.");
            bios.data.state = ParseError;
            return bios;
        }
        pos += DTS_LONG_PART3.length();

        QByteArray reversedKey = moduleBody.mid(pos, DTS_KEY_LENGTH);
        bool reversed = true;
        for(unsigned int i = 0; i < DTS_KEY_LENGTH; i++)
        {
            reversed = reversed && (bios.data.module.dts_key.at(i) == (reversedKey.at(DTS_KEY_LENGTH-1-i) ^ DTS_LONG_MASK[i]));
        }
        if(!reversed)
        {
            lastError = tr("Long DTS key reversed bytes section is corrupted.");
            bios.data.state = ParseError;
            return bios;
        }
        pos += DTS_KEY_LENGTH;

        if(moduleBody.mid(pos, DTS_LONG_PART4.length()) != DTS_LONG_PART4)
        {
            lastError = tr("Part 4 of long DTS header is unknown.");
            bios.data.state = ParseError;
            return bios;
        }
    }

    // UUID
    if(bios.mb.uuid_status == UuidPresent)
    {
        pos = moduleBody.indexOf(bios.mb.uuid_header);
        if (pos == -1)
        {
            lastError = tr("System UUID required but not found.");
            bios.data.state = ParseError;
            return bios;  
        }
        pos += bios.mb.uuid_header.length();
        bios.data.module.uuid = moduleBody.mid(pos, UUID_LENGTH);
        
        // MAC part of UUID
        if(bios.mb.mac_type == UUID || (bios.mb.mac_type == GbE && !hasGbE))
        {
            bios.data.module.mac = bios.data.module.uuid.right(MAC_LENGTH);
        }
    }

    // MBSN
    if(bios.mb.mbsn_status == MbsnPresent)
    {
        pos = moduleBody.indexOf(bios.mb.mbsn_header);
        if(pos == -1)
        {
            lastError = tr("Motherboard S/N required but not found.");
            bios.data.state = ParseError;
            return bios;
        }
        pos += bios.mb.mbsn_header.length();
        bios.data.module.mbsn = moduleBody.mid(pos, MBSN_BODY_LENGTH);
    }

    bios.data.state = Valid;
    return bios;
}

QByteArray FD44Editor::writeToBIOS(const QByteArray & data, const bios_t & bios)
{
    // Checking for module presence
    int pos = data.indexOf(MODULE_HEADER);
    if (pos == -1)
    {
        lastError = tr("FD44 module not found.");
        return QByteArray();
    }

    // Checking for BOOTEFI header
    pos = data.indexOf(BOOTEFI_HEADER);
    if (pos == -1)
    {
        lastError = tr("$BOOTEFI$ signature not found.\nPlease open correct ASUS BIOS file.");
        return QByteArray();
    }

    // Checking motherboard name
    pos += BOOTEFI_HEADER.length() + BOOTEFI_MAGIC_LENGTH + BOOTEFI_BIOS_VERSION_LENGTH;
    QByteArray motherboard_name = data.mid(pos, BOOTEFI_MOTHERBOARD_NAME_LENGTH);   
    if (!qstrcmp(bios.mb.name, motherboard_name))
    {
        lastError = tr("Motherboard model from loaded data are different from motherboard model from selected file.\n"\
                       "Loaded: %1\n"\
                       "File: %2")
                       .arg(QString(bios.mb.name))
                       .arg(QString(motherboard_name));
        return QByteArray();
    }

    QByteArray module;
    
    // MAC
    if (bios.mb.mac_type == ASCII)
    {
        module.append(bios.mb.mac_header);
        if(bios.mb.mac_header == ASCII_MAC_HEADER_7_SERIES)
        {
            module.append(bios.data.module.mac_magic ? bios.data.module.mac_magic : bios.mb.mac_magic);
            module.append('\x00');
        }
        module.append(bios.data.module.mac.toHex().toUpper());
        module.append('\x00');
    }
   
    // Short DTS key
    if(bios.mb.dts_type == Short)
    {
        module.append(bios.mb.dts_header);
        module.append(bios.data.module.dts_key);
        module.append(DTS_SHORT_PART2);
    }

    // Long DTS key
    if(bios.mb.dts_type == Long)
    {
        module.append(bios.mb.dts_header);
        module.append(bios.data.module.dts_key);
        module.append(DTS_LONG_PART2);
        module.append(bios.data.module.dts_magic.isEmpty() ? bios.mb.dts_magic : bios.data.module.dts_magic);
        module.append(DTS_LONG_PART3);
        QByteArray reversedKey;
        for(unsigned int i = 0; i < DTS_KEY_LENGTH; i++)
            reversedKey.append(bios.data.module.dts_key.at(DTS_KEY_LENGTH-1-i) ^ DTS_LONG_MASK[i]);
        module.append(reversedKey);
        module.append(DTS_LONG_PART4);
    }

    // UUID
    if (bios.mb.uuid_status == UuidPresent)
    {
        module.append(bios.mb.uuid_header);
        module.append(bios.data.module.uuid);
        if(bios.mb.mac_type == GbE)
            module.append(bios.data.gbe.mac);
        else
            module.append(bios.data.module.mac);
    }

    // MBSN
    if (bios.mb.mbsn_status == MbsnPresent)
    {
        module.append(bios.mb.mbsn_header);
        module.append(bios.data.module.mbsn);
        module.append('\x00');
    }

    // Replacing all modules
    QByteArray newData = data;
    int moduleLength;
    pos = data.indexOf(MODULE_HEADER);
    while(pos != -1)
    {
        // Checking for BSA_ signature
        if(data.mid(pos + MODULE_HEADER_BSA_OFFSET, MODULE_HEADER_BSA.length()) != MODULE_HEADER_BSA)
        {
            pos = data.indexOf(MODULE_HEADER, pos + MODULE_HEADER_LENGTH);
            continue;
        }
        
        // Reading module length
        moduleLength = (data.at(pos + MODULE_LENGTH_OFFSET + 2) << 16) +
                       (data.at(pos + MODULE_LENGTH_OFFSET + 1) << 8)  +
                        data.at(pos + MODULE_LENGTH_OFFSET);
        if(moduleLength - MODULE_HEADER_LENGTH < module.length())
        {
            lastError = tr("FD44 module in output file is too small to insert all data.\n Please use another full BIOS backup or factory BIOS file.");
            return QByteArray();
        }
        
        // Replacing module data
        pos += MODULE_HEADER_LENGTH;
        newData.replace(pos, module.length(), module);
        
        // Inserting FF bytes to the end of the module
        pos += module.length();
        QByteArray ffs(moduleLength - MODULE_HEADER_LENGTH - module.length(), '\xFF');
        newData.replace(pos, ffs.length(), ffs);
        
        // Going to the next module
        pos = data.indexOf(MODULE_HEADER, pos);
    }

    // Checking for descriptor header in modified file
    if(newData.left(DESCRIPTOR_HEADER_COMMON.size()) != DESCRIPTOR_HEADER_COMMON 
    && newData.left(DESCRIPTOR_HEADER_RARE.size()) != DESCRIPTOR_HEADER_RARE)
    {
        lastError = tr("Descriptor header is unknown.");
        return QByteArray();
    }

    if(bios.mb.mac_type != GbE)
        return newData;

    // Replacing GbE MACs
    pos = newData.indexOf(GBE_HEADER);
    int pos2 = newData.lastIndexOf(GBE_HEADER);
    if (pos == -1)
    {
        lastError = tr("GbE region not found in selected file.\n Please use full BIOS backup or factory BIOS file.");
        return QByteArray();
    }

    newData.replace(pos + GBE_MAC_OFFSET - MAC_LENGTH, MAC_LENGTH, bios.data.gbe.mac);
    newData.replace(pos2 + GBE_MAC_OFFSET - MAC_LENGTH, MAC_LENGTH, bios.data.gbe.mac);
    return newData;
}

bool FD44Editor::writeToUI(bios_t bios)
{
    switch (bios.data.state)
    {
    case ParseError:
        QMessageBox::critical(this, tr("Fatal error"), tr("Error parsing BIOS data.\n%1").arg(lastError));
        return false;
    case Empty:
        QMessageBox::information(this, tr("Loaded module is empty"), tr("Loaded module is empty.\nIt is normal, if you are opening BIOS file downloaded from asus.com\n"\
                                                                        "If you are opening system BIOS image, you must restore module data in your BIOS.\n"));
        break;
    case Valid:
        break;
    }

    ui->mbEdit->setText(bios.data.be.motherboard_name);
    ui->biosVersionEdit->setText(QString("%1%2").arg((int)bios.data.be.bios_version.at(0),2,10,QChar('0')).arg((int)bios.data.be.bios_version.at(1),2,10,QChar('0')));
    ui->dateEdit->setText(bios.data.be.bios_date);

    // ME version
    if (!bios.data.me.me_version.isEmpty())
    {
        if(bios.data.me.me_version.length() == ME_VERSION_LENGTH)
        {
            qint16 major =  *(qint16*)(const void*)(bios.data.me.me_version.mid(0, 2));
            qint16 minor =  *(qint16*)(const void*)(bios.data.me.me_version.mid(2, 2));
            qint16 bugfix = *(qint16*)(const void*)(bios.data.me.me_version.mid(4, 2));
            qint16 build =  *(qint16*)(const void*)(bios.data.me.me_version.mid(6, 2));
            ui->meVersionEdit->setText(QString("%1.%2.%3.%4").arg(major).arg(minor).arg(bugfix).arg(build));
        }
        else
            ui->meVersionEdit->setText(tr("Not detected"));
    }
    else
        ui->meVersionEdit->setText("Not present");

    //GbE version
    if (!bios.data.gbe.gbe_version.isEmpty())
    {
        quint8 major = bios.data.gbe.gbe_version.at(1);
        quint8 minor = bios.data.gbe.gbe_version.at(0) >> 4 & 0x0F;
        //quint8 image_id = data.gbe.gbe_version.at(0) & 0x0F;
        ui->gbeVersionEdit->setText(QString("%1.%2").arg(major).arg(minor));
    }
    else
        ui->gbeVersionEdit->setText(tr("Not present"));

    if (bios.mb.mac_type == GbE && !bios.data.gbe.mac.isEmpty())
    {
        ui->lanEdit->setText(tr("Detected from GbE region"));
        ui->macEdit->setText(bios.data.gbe.mac.toHex());
        ui->macEdit->setEnabled(true);
    }
    else
    {
        ui->lanEdit->setText(tr("Detected from module"));
        ui->macEdit->setText(bios.data.module.mac.toHex());
        ui->macEdit->setEnabled(true);
    }

    if (bios.mb.dts_type == None)
    {
        ui->dtsEdit->setText(tr("Detected from module"));
        ui->dtsKeyEdit->setText("");
        ui->dtsKeyEdit->setEnabled(false);
    }
    else
    {
        ui->dtsEdit->setText(tr("Detected from module"));
        ui->dtsKeyEdit->setText(bios.data.module.dts_key.toHex());
        ui->dtsKeyEdit->setEnabled(true);
    }

    if(bios.mb.uuid_status == UuidPresent)
    {
        ui->uuidEdit->setText(bios.data.module.uuid.toHex());
        ui->uuidEdit->setEnabled(true);
    }

    if(bios.mb.mbsn_status == MbsnPresent)
    {
        ui->mbsnEdit->setText(bios.data.module.mbsn);
        ui->mbsnEdit->setEnabled(true);
    }
    
    ui->copyButton->setEnabled(true);
    opened = bios;
    return true;
}

bios_t FD44Editor::readFromUI()
{
    bios_t bios = opened;

    bios.data.gbe.mac = QByteArray::fromHex(ui->macEdit->text().toAscii());
    bios.data.module.mac = QByteArray::fromHex(ui->macEdit->text().toAscii());
    bios.data.module.uuid = QByteArray::fromHex(ui->uuidEdit->text().toAscii());
    bios.data.module.dts_key = QByteArray::fromHex(ui->dtsKeyEdit->text().toAscii());
    bios.data.module.mbsn = ui->mbsnEdit->text().toAscii();
    
    return bios;
}

void FD44Editor::enableSaveButton()
{
    if (ui->uuidEdit->text().length() == ui->uuidEdit->maxLength()
        && ui->macEdit->text().length() == ui->macEdit->maxLength()
        && ui->mbsnEdit->text().length() == ui->mbsnEdit->maxLength())
    {
        if (ui->dtsKeyEdit->isEnabled() && ui->dtsKeyEdit->text().length() != ui->dtsKeyEdit->maxLength())
            ui->toFileButton->setEnabled(false);
        else
            ui->toFileButton->setEnabled(true);
    }
    else
        ui->toFileButton->setEnabled(false);
}

void FD44Editor::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
             event->acceptProposedAction();
}

void FD44Editor::dropEvent(QDropEvent* event)
{
    QString path = event->mimeData()->urls().at(0).toLocalFile();
    openImageFile(path);
}
