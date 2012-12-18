/* fd44editor.cpp

  Copyright (c) 2012, Nikolaj Schlej. All rights reserved.
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

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
	connect(ui->toClipboardButton, SIGNAL(clicked()), this, SLOT(copyToClipboard()));
    connect(ui->uuidEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButton()));
    connect(ui->macEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButton()));
    connect(ui->mbsnEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButton()));
    connect(ui->dtsKeyEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButton()));
    connect(ui->macMagicEdit, SIGNAL(textChanged(QString)), this, SLOT(enableSaveButton()));
    connect(ui->macStorageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableMacMagicEdit(int)));
    connect(ui->dtsTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableDtsMagicCombobox(int)));
    connect(ui->dtsTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableDtsKeyEdit(int)));

    // Enable Drag-and-Drop actions
    this->setAcceptDrops(true);

    // Populating DTS magic combobox with data
    ui->dtsMagicComboBox->setItemData(0, DTS_LONG_MAGIC_V1);
    ui->dtsMagicComboBox->setItemData(1, DTS_LONG_MAGIC_V2);
    ui->dtsMagicComboBox->setItemData(2, DTS_LONG_MAGIC_V3);
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
    if (!fileInfo.exists())
    {
        ui->statusBar->showMessage(tr("Please select existing BIOS image file."));
        return;
    }

    QFile inputFile;
    inputFile.setFileName(path);

    if (!inputFile.open(QFile::ReadOnly))
    {
        ui->statusBar->showMessage(tr("Can't open file for reading. Check file permissions."));
        return;
    }

    QByteArray biosImage = inputFile.readAll();
    inputFile.close();

    if (writeToUI(readFromBIOS(biosImage)))
        ui->statusBar->showMessage(tr("Loaded: %1").arg(fileInfo.fileName()));

	ui->toClipboardButton->setEnabled(true);
}

void FD44Editor::saveImageFile()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save BIOS image file"),".","BIOS image file (*.rom *.bin *.cap);;All files (*.*)");

    QFileInfo fileInfo = QFileInfo(path);
    if (!fileInfo.exists())
    {
        ui->statusBar->showMessage(tr("Please select existing BIOS image file."));
        return;
    }

    QFile outputFile;
    outputFile.setFileName(path);
    if (!outputFile.open(QFile::ReadWrite))
    {
        ui->statusBar->showMessage(tr("Can't open file for writing. Check file permissions."));
        return;
    }

    QByteArray bios = outputFile.readAll();

    if (bios.left(UBF_FILE_HEADER.length()) == UBF_FILE_HEADER)
    {
        bios = bios.mid(UBF_FILE_HEADER_SIZE); 
    }

    QByteArray newBios = writeToBIOS(bios, readFromUI());
    if (newBios.isEmpty())
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

bios_t FD44Editor::readFromBIOS(const QByteArray & data)
{
    bios_t bios;

	// Setting default values
	bios.mac_type = MacNotDetected;

    // Detecting motherboard model and BIOS version
    int pos = data.lastIndexOf(BOOTEFI_HEADER);
    if (pos == -1)
    {
        lastError = tr("$BOOTEFI$ signature not found.\nPlease open correct ASUS BIOS file.");
        bios.state = ParseError;
        return bios;
    }

    pos += BOOTEFI_HEADER.length() + BOOTEFI_MAGIC_LENGTH;
    bios.bios_version = data.mid(pos, BOOTEFI_BIOS_VERSION_LENGTH);
    pos += BOOTEFI_BIOS_VERSION_LENGTH;
    bios.motherboard_name = data.mid(pos, BOOTEFI_MOTHERBOARD_NAME_LENGTH);
    pos += BOOTEFI_MOTHERBOARD_NAME_LENGTH + BOOTEFI_BIOS_DATE_OFFSET;
    bios.bios_date = data.mid(pos, BOOTEFI_BIOS_DATE_LENGTH);

    // Searching for that board in database
    int dbIndex = -1;

    for(int i = 0; i < SUPPORTED_MOTHERBOARDS_LIST_LENGTH; i++)
    {
        QByteArray motherboard_name = QByteArray(SUPPORTED_MOTHERBOARDS_LIST[i].name, bios.motherboard_name.length());
        if (!qstrcmp(motherboard_name, bios.motherboard_name))
        {
            dbIndex = i;
            break;
        }
    }

    // Detecting ME presence and version
    bool isFull = false;
	pos = data.indexOf(ME_HEADER);
    if (pos != -1)
    {
        if (data.indexOf(ME_5M_SIGN, pos) != -1)
			bios.me_type = ME_5M;
		else if (data.indexOf(ME_3M_SIGN, pos) != -1)
			bios.me_type = ME_3M;
		else 
			bios.me_type = ME_15M;

		pos = data.indexOf(ME_VERSION_HEADER, pos);
        if (pos != -1)
        {
			bios.me_version = data.mid(pos + ME_VERSION_HEADER.length() + ME_VERSION_OFFSET, ME_VERSION_LENGTH);
			isFull = true;
        }
    }

    // Detecting GbE presence and version
    bool macFound = false;
    pos = data.indexOf(GBE_HEADER);
    if (pos != -1)
    {
        int pos2 = data.lastIndexOf(GBE_HEADER);
        if (pos != pos2 && data.mid(pos + GBE_MAC_OFFSET - MAC_LENGTH, MAC_LENGTH) == GBE_MAC_STUB)
            pos = pos2;

        bios.mac = data.mid(pos + GBE_MAC_OFFSET - MAC_LENGTH, MAC_LENGTH);
        bios.gbe_version = data.mid(pos + GBE_VERSION_OFFSET, GBE_VERSION_LENGTH);
        bios.mac_type = GbE;
        macFound = true;
    }

    // Searching for non-empty module
    pos = data.indexOf(MODULE_HEADER);
    if (pos == -1)
    {
        lastError = tr("FD44 module not found.");
        bios.state = ParseError;
        return bios;
    }

    bool isEmpty = true;
    unsigned int moduleLength;
    QByteArray module, moduleBody, moduleVersion;
    while (isEmpty && pos != -1)
    {
        // Checking for BSA_ signature
        if (data.mid(pos + MODULE_HEADER_BSA_OFFSET, MODULE_HEADER_BSA.length()) != MODULE_HEADER_BSA)
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
        moduleVersion = module.mid(MODULE_VERSION_OFFSET, MODULE_VERSION_LENGTH);
        if (MODULE_VERSIONS.indexOf(moduleVersion) < 0)
        {
            lastError = tr("FD44 module version is unknown.");
            bios.state = ParseError;
            return bios;
        }

        // Setting up module structure depending on detected module version
        // X79 motherboards have similar FD44 module header, but different data format.
        bool x79board = (bios.motherboard_name.indexOf("X79") != -1 || bios.motherboard_name.indexOf("Rampage-IV") != -1);
        
		// C20x motherboards have similar FD44 module header, but different data format.
		// TODO: replace detection algorithm, too many exclusions
		bool c20xboard = (bios.motherboard_name.indexOf("P8B-") != -1);
		
		bios.module_version = moduleVersion;
        switch (MODULE_VERSIONS.indexOf(bios.module_version))
        {
        case 0: // 6 series or X79 or C20x
            if (x79board) // X79
            {
                bios.mac_header = QByteArray();
                bios.dts_short_header = QByteArray();
                bios.dts_long_header = DTS_LONG_HEADER_X79;
                bios.mbsn_header = MBSN_HEADER_X79;
                bios.uuid_header = UUID_HEADER_X79;
            }
			else if (c20xboard)	// C20x
			{
				bios.mac_header = QByteArray();
				bios.dts_short_header = QByteArray();
				bios.dts_long_header = QByteArray();
				bios.mbsn_header = MBSN_HEADER_7_SERIES;
				bios.uuid_header = UUID_HEADER_7_SERIES;
			}
			else // 6 series
			{
                bios.mac_header = ASCII_MAC_HEADER_6_SERIES;
                bios.dts_short_header = DTS_SHORT_HEADER_6_SERIES;
                bios.dts_long_header = DTS_LONG_HEADER_6_SERIES;
                bios.mbsn_header = MBSN_HEADER_6_SERIES;
                bios.uuid_header = UUID_HEADER_6_SERIES;
            }
            break;
        case 1: // C602
            bios.mac_header = QByteArray();
            bios.dts_short_header = QByteArray();
            bios.dts_long_header = QByteArray();
            bios.mbsn_header = MBSN_HEADER_7_SERIES;
            bios.uuid_header = UUID_HEADER_7_SERIES;
            break;
        case 2: // 7 series
            bios.mac_header = ASCII_MAC_HEADER_7_SERIES;
            bios.dts_short_header = QByteArray();
            bios.dts_long_header = DTS_LONG_HEADER_7_SERIES;
            bios.mbsn_header = MBSN_HEADER_7_SERIES;
            bios.uuid_header = UUID_HEADER_7_SERIES;
            break;
        default:
            lastError = tr("No valid structure setup path for this module version.");
            bios.state = ParseError;
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
        // Trying to detect module data format from board database
        if (dbIndex >= 0)
        {
            bios.mac_type = SUPPORTED_MOTHERBOARDS_LIST[dbIndex].mac_type;
            bios.mac_magic = SUPPORTED_MOTHERBOARDS_LIST[dbIndex].mac_magic;
            bios.dts_type = SUPPORTED_MOTHERBOARDS_LIST[dbIndex].dts_type;
            bios.dts_magic = SUPPORTED_MOTHERBOARDS_LIST[dbIndex].dts_magic;
            bios.state = Empty;
        }
        else
        {
            bios.mac_magic = QByteArray();
            bios.dts_type = DtsNotDetected;
            bios.dts_magic = QByteArray();
            bios.state = HasNotDetectedValues;
        }

        return bios;
    }

    // Detecting MAC address type and value
    // Searching for ASCII MAC
    if (!bios.mac_header.isEmpty() && bios.mac_type != GbE)
    {
        pos = moduleBody.indexOf(bios.mac_header);
        if (pos != -1 )
        {
            pos += bios.mac_header.length();

            if (bios.mac_header == ASCII_MAC_HEADER_7_SERIES)
            {
                bios.mac_magic = moduleBody.mid(pos, ASCII_MAC_MAGIC_LENGTH);
                pos += ASCII_MAC_OFFSET;
            }

            bios.mac = QByteArray::fromHex(moduleBody.mid(pos, ASCII_MAC_LENGTH));
            bios.mac_type = ASCII;
            macFound = true;
        }
    }

    if (!macFound)
    {
        if (dbIndex >= 0)
        {
            bios.mac_type = SUPPORTED_MOTHERBOARDS_LIST[dbIndex].mac_type;
            bios.mac_magic = SUPPORTED_MOTHERBOARDS_LIST[dbIndex].mac_magic;
        }
        else
        {
            bios.mac_type = MacNotDetected;
            bios.mac_magic = QByteArray();
        }
    }
    
    // Searching for DTS key
    bool dtsFound = false;
    // Searching for short DTS
    if (!bios.dts_short_header.isEmpty())
    {
        pos = moduleBody.indexOf(bios.dts_short_header);
        if (pos != -1)
        {
            pos += bios.dts_short_header.length();
            bios.dts_key = moduleBody.mid(pos, DTS_KEY_LENGTH);
            pos += DTS_KEY_LENGTH;

            if (moduleBody.mid(pos, DTS_SHORT_PART2.length()) != DTS_SHORT_PART2)
            {
                lastError = tr("Part 2 of short DTS key is unknown.");
                bios.state = ParseError;
                return bios;
            }

            bios.dts_type = Short;
            dtsFound = true;
        }
    }

    // Searching for long DTS
    if (bios.dts_type != Short && !bios.dts_long_header.isEmpty())
    {
        pos = moduleBody.indexOf(bios.dts_long_header);
        if (pos != -1)
        {
            pos += bios.dts_long_header.length();
            bios.dts_key = moduleBody.mid(pos, DTS_KEY_LENGTH);
            pos += DTS_KEY_LENGTH;

            if (moduleBody.mid(pos, DTS_LONG_PART2.length()) !=DTS_LONG_PART2)
            {
                lastError = tr("Part 2 of long DTS key is unknown.");
                bios.state = ParseError;
                return bios;
            }
            pos += DTS_LONG_PART2.length();

            bios.dts_magic = moduleBody.mid(pos, DTS_LONG_MAGIC_LENGTH);
            pos += DTS_LONG_MAGIC_LENGTH;

            if (moduleBody.mid(pos, DTS_LONG_PART3.length()) != DTS_LONG_PART3)
            {
                lastError = tr("Part 3 of long DTS key is unknown.");
                bios.state = ParseError;
                return bios;
            }
            pos += DTS_LONG_PART3.length();

            QByteArray reversedKey = moduleBody.mid(pos, DTS_KEY_LENGTH);
            bool reversed = true;
            for(unsigned int i = 0; i < DTS_KEY_LENGTH; i++)
            {
                reversed = reversed && (bios.dts_key.at(i) == (reversedKey.at(DTS_KEY_LENGTH-1-i) ^ DTS_LONG_MASK[i]));
            }
            if (!reversed)
            {
                lastError = tr("Long DTS key reversed bytes section is corrupted.");
                bios.state = ParseError;
                return bios;
            }
            pos += DTS_KEY_LENGTH;

            if (moduleBody.mid(pos, DTS_LONG_PART4.length()) != DTS_LONG_PART4)
            {
                lastError = tr("Part 4 of long DTS header is unknown.");
                bios.state = ParseError;
                return bios;
            }

            bios.dts_type = Long;
            dtsFound = true;
        }
    }

    if (!dtsFound)
    {
        if (dbIndex >= 0)
        {
            bios.dts_type = SUPPORTED_MOTHERBOARDS_LIST[dbIndex].dts_type;
            bios.dts_magic = SUPPORTED_MOTHERBOARDS_LIST[dbIndex].dts_magic;
        }
        else
        {
            bios.dts_type = DtsNotDetected;
            bios.dts_magic = QByteArray();
        }
    }

    // Searching for UUID
    if (!bios.uuid_header.isEmpty())
    {
        pos = moduleBody.indexOf(bios.uuid_header);
        if (pos == -1)
        {
            lastError = tr("System UUID required but not found.");
            bios.state = ParseError;
            return bios;  
        }
        pos += bios.uuid_header.length();
        bios.uuid = moduleBody.mid(pos, UUID_LENGTH);
        
        // MAC part of UUID
        if (!macFound || bios.mac_type == UUID)
        {
            bios.mac = bios.uuid.right(MAC_LENGTH);
        }
    }

    // Searching for MBSN
    if (!bios.mbsn_header.isEmpty())
    {
        pos = moduleBody.indexOf(bios.mbsn_header);
        if (pos == -1)
        {
            lastError = tr("Motherboard S/N required but not found.");
            bios.state = ParseError;
            return bios;
        }
        pos += bios.mbsn_header.length();
        bios.mbsn = moduleBody.mid(pos, MBSN_BODY_LENGTH);
    }

    // Checking for not detected values
    if (bios.mac_type == MacNotDetected || bios.dts_type == DtsNotDetected)
        bios.state = HasNotDetectedValues;
    else
        bios.state = Valid;

    return bios;
}

QByteArray FD44Editor::writeToBIOS(const QByteArray & data, const bios_t & bios)
{
    // Checking for BOOTEFI header
    int pos = data.indexOf(BOOTEFI_HEADER);
    if (pos == -1)
    {
        lastError = tr("$BOOTEFI$ signature not found in output file.\nPlease open correct ASUS BIOS file.");
        return QByteArray();
    }

    // Checking for module presence
    pos = data.indexOf(MODULE_HEADER);
    if (pos == -1)
    {
        lastError = tr("FD44 module not found in output file.");
        return QByteArray();
    }

    // Checking motherboard name
    pos += BOOTEFI_HEADER.length() + BOOTEFI_MAGIC_LENGTH + BOOTEFI_BIOS_VERSION_LENGTH;
    QByteArray motherboard_name = data.mid(pos, BOOTEFI_MOTHERBOARD_NAME_LENGTH);   
    if (!qstrcmp(bios.motherboard_name, motherboard_name))
    {
        lastError = tr("Motherboard model in in output file differs from model in loaded data.\n"\
                       "Loaded: %1\n"\
                       "File: %2")
                       .arg(QString(bios.motherboard_name))
                       .arg(QString(motherboard_name));
        return QByteArray();
    }

    QByteArray module;
    
    // MAC
    if (bios.mac_type == ASCII)
    {
        module.append(bios.mac_header);
        if (bios.mac_header == ASCII_MAC_HEADER_7_SERIES)
        {
            module.append(bios.mac_magic);
            module.append('\x00');
        }
        module.append(bios.mac.toHex().toUpper());
        module.append('\x00');
    }
   
    // Short DTS key
    if (bios.dts_type == Short)
    {
        module.append(bios.dts_short_header);
        module.append(bios.dts_key);
        module.append(DTS_SHORT_PART2);
    }

    // Long DTS key
    if (bios.dts_type == Long)
    {
        module.append(bios.dts_long_header);
        module.append(bios.dts_key);
        module.append(DTS_LONG_PART2);
        module.append(bios.dts_magic);
        module.append(DTS_LONG_PART3);
        QByteArray reversedKey;
        for(unsigned int i = 0; i < DTS_KEY_LENGTH; i++)
            reversedKey.append(bios.dts_key.at(DTS_KEY_LENGTH-1-i) ^ DTS_LONG_MASK[i]);
        module.append(reversedKey);
        module.append(DTS_LONG_PART4);
    }

    // UUID
    if (!bios.uuid_header.isEmpty())
    {
        module.append(bios.uuid_header);
        module.append(bios.uuid);
        module.append(bios.mac);
    }

    // MBSN
    if (!bios.mbsn_header.isEmpty())
    {
        module.append(bios.mbsn_header);
        module.append(bios.mbsn);
        module.append('\x00');
    }

    // Replacing all modules
    QByteArray newData = data;
    QByteArray moduleVersion;
    int moduleLength;
    pos = data.indexOf(MODULE_HEADER);
    while(pos != -1)
    {
        // Checking for BSA_ signature
        if (data.mid(pos + MODULE_HEADER_BSA_OFFSET, MODULE_HEADER_BSA.length()) != MODULE_HEADER_BSA)
        {
            pos = data.indexOf(MODULE_HEADER, pos + MODULE_HEADER_LENGTH);
            continue;
        }
        
        // Reading module length
        moduleLength = (data.at(pos + MODULE_LENGTH_OFFSET + 2) << 16) +
                       (data.at(pos + MODULE_LENGTH_OFFSET + 1) << 8)  +
                        data.at(pos + MODULE_LENGTH_OFFSET);
        if (moduleLength - MODULE_HEADER_LENGTH < module.length())
        {
            lastError = tr("FD44 module in output file is too small to insert all data.\n Please use another full BIOS backup or factory BIOS file.");
            return QByteArray();
        }
        
        // Checking module version
        moduleVersion = data.mid(pos + MODULE_VERSION_OFFSET, MODULE_VERSION_LENGTH);
        if (MODULE_VERSIONS.indexOf(moduleVersion) < 0)
        {
            lastError = tr("FD44 module version in output file is unknown.");
            return QByteArray();
        }
        if (moduleVersion != bios.module_version)
        {
            lastError = tr("FD44 module version in output file differs from version in input file.");
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

    // Replacing GbE MACs
    if (bios.mac_type == GbE)
    {
        pos = newData.indexOf(GBE_HEADER);
        int pos2 = newData.lastIndexOf(GBE_HEADER);
        if (pos == -1)
        {
            lastError = tr("GbE region is set as MAC storage but not found in output file.");
            return QByteArray();
        }
        newData.replace(pos + GBE_MAC_OFFSET - MAC_LENGTH, MAC_LENGTH, bios.mac);
        newData.replace(pos2 + GBE_MAC_OFFSET - MAC_LENGTH, MAC_LENGTH, bios.mac);
    }

    // Checking for descriptor header in modified file
    if (newData.left(DESCRIPTOR_HEADER_COMMON.size()) != DESCRIPTOR_HEADER_COMMON
    && newData.left(DESCRIPTOR_HEADER_RARE.size()) != DESCRIPTOR_HEADER_RARE)
    {
        lastError = tr("Descriptor header is unknown.");
        return QByteArray();
    }

    return newData;
}

bool FD44Editor::writeToUI(bios_t bios)
{
    switch (bios.state)
    {
    case ParseError:
        QMessageBox::critical(this, tr("Fatal error"), tr("Error parsing BIOS data.\n%1").arg(lastError));
        return false;
    case Empty:
        QMessageBox::information(this, tr("Loaded module is empty"), tr("Loaded module is empty.\nIt is normal, if you are opening BIOS file downloaded from asus.com\n"\
                                                                        "If you are opening system BIOS image, you must restore module data in your BIOS.\n"));
        break;
    case HasNotDetectedValues:
        QMessageBox::information(this, tr("Data format can't be detected"), tr("Module data format in opened file can't be fully detected.\n"\
                                                                               "It is normal, if you are opening BIOS backup made by ASUS tools.\n"\
                                                                               "Please setup data format manually.\n"));
        if (bios.mac_type == MacNotDetected)
        {
            bios.mac_type = UUID;
            bios.mac_magic = QByteArray();
        }
        if (bios.dts_type == DtsNotDetected)
        {
            bios.dts_type = None;
            bios.dts_magic = QByteArray();
        }
        break;
    case Valid:
        break;
    }

    // BIOS information
    ui->mbEdit->setText(bios.motherboard_name);
    ui->biosVersionEdit->setText(QString("%1%2").arg((int)bios.bios_version.at(0),2,10,QChar('0')).arg((int)bios.bios_version.at(1),2,10,QChar('0')));
    ui->dateEdit->setText(bios.bios_date);

    // ME version
    if (!bios.me_version.isEmpty())
    {
        if (bios.me_version.length() == ME_VERSION_LENGTH)
        {
            qint16 major =  *(qint16*)(const void*)(bios.me_version.mid(0, 2));
            qint16 minor =  *(qint16*)(const void*)(bios.me_version.mid(2, 2));
            qint16 bugfix = *(qint16*)(const void*)(bios.me_version.mid(4, 2));
            qint16 build =  *(qint16*)(const void*)(bios.me_version.mid(6, 2));
            QString me;
			if (bios.me_type == ME_5M)
				me = "5M";
			else if (bios.me_type == ME_3M)
				me = "3M";
			else
				me = "1.5M";
			ui->meVersionEdit->setText(QString("%1.%2.%3.%4 (%5)").arg(major).arg(minor).arg(bugfix).arg(build).arg(me));
        }
        else
            ui->meVersionEdit->setText(tr("Not detected"));
    }
    else
        ui->meVersionEdit->setText("Not present");

    // GbE version
    if (!bios.gbe_version.isEmpty())
    {
        quint8 major = bios.gbe_version.at(1);
        quint8 minor = bios.gbe_version.at(0) >> 4 & 0x0F;
        //quint8 image_id = data.gbe.gbe_version.at(0) & 0x0F;
        ui->gbeVersionEdit->setText(QString("%1.%2").arg(major).arg(minor));
    }
    else
        ui->gbeVersionEdit->setText(tr("Not present"));

    // Comboboxes
    // MAC storage
    ui->macStorageComboBox->clear();
    ui->macStorageComboBox->addItem("System UUID only", UUID);
    if (!bios.mac_header.isEmpty())
        ui->macStorageComboBox->addItem("ASCII string and system UUID", ASCII);
    ui->macStorageComboBox->addItem("GbE region and system UUID", GbE);

    // DTS type
    ui->dtsTypeComboBox->setEnabled(true);
	ui->dtsTypeComboBox->clear();
    ui->dtsTypeComboBox->addItem("None", None);
    if (!bios.dts_short_header.isEmpty())
        ui->dtsTypeComboBox->addItem("Short", Short);
    if (!bios.dts_long_header.isEmpty())
        ui->dtsTypeComboBox->addItem("Long", Long);
	if (ui->dtsTypeComboBox->count() == 1)
		ui->dtsTypeComboBox->setEnabled(false);

    // MAC
    switch (bios.mac_type)
    {
    case UUID:
        ui->macStorageComboBox->setCurrentIndex(ui->macStorageComboBox->findData(UUID));
        ui->macMagicEdit->setText("");
        ui->macMagicEdit->setEnabled(false);
        break;
    case ASCII:
        ui->macStorageComboBox->setCurrentIndex(ui->macStorageComboBox->findData(ASCII));
        if (bios.mac_header == ASCII_MAC_HEADER_7_SERIES)
        {
            ui->macMagicEdit->setText(bios.mac_magic.toHex());
            ui->macMagicEdit->setEnabled(true);
        }
        else
        {
            ui->macMagicEdit->setText("");
            ui->macMagicEdit->setEnabled(false);
        }
        break;
    case GbE:
        ui->macStorageComboBox->setCurrentIndex(ui->macStorageComboBox->findData(GbE));
        ui->macMagicEdit->setText("");
        ui->macMagicEdit->setEnabled(false);
        break;
    default:
        QMessageBox::critical(this, tr("Fatal error"), tr("Undefined control path in MAC setup.\n%1").arg(lastError));
        return false;
    }
    ui->macEdit->setText(bios.mac.toHex());
    ui->macEdit->setEnabled(true);
    ui->macStorageComboBox->setEnabled(true);

    // DTS key
    switch (bios.dts_type)
    {
    case None:
        ui->dtsKeyEdit->setText("");
        ui->dtsKeyEdit->setEnabled(false);
        ui->dtsTypeComboBox->setCurrentIndex(ui->macStorageComboBox->findData(None));
        ui->dtsMagicComboBox->setCurrentIndex(ui->macStorageComboBox->findData(DTS_LONG_MAGIC_V1));
        ui->dtsMagicComboBox->setEnabled(false);
        break;
    case Short:
        ui->dtsKeyEdit->setText(bios.dts_key.toHex());
        ui->dtsKeyEdit->setEnabled(true);
        ui->dtsTypeComboBox->setCurrentIndex(ui->macStorageComboBox->findData(Short));
        ui->dtsMagicComboBox->setCurrentIndex(ui->macStorageComboBox->findData(DTS_LONG_MAGIC_V1));
        ui->dtsMagicComboBox->setEnabled(false);
        break;
    case Long:
        ui->dtsKeyEdit->setText(bios.dts_key.toHex());
        ui->dtsKeyEdit->setEnabled(true);
        ui->dtsTypeComboBox->setCurrentIndex(ui->macStorageComboBox->findData(Long));
        ui->dtsMagicComboBox->setCurrentIndex(ui->dtsMagicComboBox->findData(bios.dts_magic));
        ui->dtsMagicComboBox->setEnabled(true);
        break;
    default:
        QMessageBox::critical(this, tr("Fatal error"), tr("Undefined control path in DTS key setup.\n%1").arg(lastError));
        return false;
    }

    // UUID
    if (!bios.uuid_header.isEmpty())
    {
        ui->uuidEdit->setText(bios.uuid.toHex());
        ui->uuidEdit->setEnabled(true);
    }

    // MBSN
    if (!bios.mbsn_header.isEmpty())
    {
        ui->mbsnEdit->setText(bios.mbsn);
        ui->mbsnEdit->setEnabled(true);
    }
    
    opened = bios;
    return true;
}

bios_t FD44Editor::readFromUI()
{
    bios_t bios = opened;

    bios.mac = QByteArray::fromHex(ui->macEdit->text().toLatin1());
    bios.uuid = QByteArray::fromHex(ui->uuidEdit->text().toLatin1());
    bios.dts_key = QByteArray::fromHex(ui->dtsKeyEdit->text().toLatin1());
    bios.mbsn = ui->mbsnEdit->text().toLatin1();
    bios.mac_type = (mac_e)ui->macStorageComboBox->itemData(ui->macStorageComboBox->currentIndex()).toInt();
    bios.mac_magic = QByteArray::fromHex(ui->macMagicEdit->text().toLatin1());
    bios.dts_type = (dts_e)ui->dtsTypeComboBox->itemData(ui->dtsTypeComboBox->currentIndex()).toInt();
    bios.dts_magic = ui->dtsMagicComboBox->itemData(ui->dtsMagicComboBox->currentIndex()).toByteArray();
    
    return bios;
}

void FD44Editor::enableSaveButton()
{
    if (ui->uuidEdit->text().length() == ui->uuidEdit->maxLength()
        && ui->macEdit->text().length() == ui->macEdit->maxLength()
        && ui->mbsnEdit->text().length() == ui->mbsnEdit->maxLength()
        && (ui->dtsKeyEdit->isEnabled() ? ui->dtsKeyEdit->text().length() == ui->dtsKeyEdit->maxLength() : true)
        && (ui->macMagicEdit->isEnabled() ? ui->macMagicEdit->text().length() == ui->macMagicEdit->maxLength() : true))
    {
        ui->toFileButton->setEnabled(true);
    }
    else
        ui->toFileButton->setEnabled(false);
}

void FD44Editor::enableDtsKeyEdit(int index)
{
    if (index > 0)
        ui->dtsKeyEdit->setEnabled(true);
    else
        ui->dtsKeyEdit->setEnabled(false);

    enableSaveButton();
}

void FD44Editor::enableMacMagicEdit(int index)
{
    if (ui->macStorageComboBox->itemData(index) == ASCII && opened.mac_header == ASCII_MAC_HEADER_7_SERIES)
       ui->macMagicEdit->setEnabled(true);
    else
        ui->macMagicEdit->setEnabled(false);

    enableSaveButton();
}

void FD44Editor::enableDtsMagicCombobox(int index)
{
    if (ui->dtsTypeComboBox->itemData(index) == Long)
    {
        ui->dtsMagicComboBox->setCurrentIndex(ui->dtsMagicComboBox->findData(opened.dts_magic.isEmpty() ? DTS_LONG_MAGIC_V1 : opened.dts_magic));
        ui->dtsMagicComboBox->setEnabled(true);
    }
    else
        ui->dtsMagicComboBox->setEnabled(false);
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

void FD44Editor::copyToClipboard()
{
	QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(tr("Motherboard name: %1\n"\
                          "BIOS date: %2\n"\
                          "BIOS version: %3\n"\
                          "ME version: %4\n"\
                          "GbE version: %5\n"\
                          "Primary LAN MAC: %6\n"\
                          "DTS key: %7\n"
                          "UUID: %8\n"\
                          "MBSN: %9")
                       .arg(ui->mbEdit->text())
                       .arg(ui->dateEdit->text())
                       .arg(ui->biosVersionEdit->text())
                       .arg(ui->meVersionEdit->text())
                       .arg(ui->gbeVersionEdit->text())
                       .arg(ui->macEdit->text().remove(':'))
                       .arg(ui->dtsKeyEdit->text().remove(' ').isEmpty() ? tr("Not present") : ui->dtsKeyEdit->text().remove(' '))
                       .arg(ui->uuidEdit->text().remove(' ').append(ui->macEdit->text().remove(':')))
                       .arg(ui->mbsnEdit->text())
                       );
}