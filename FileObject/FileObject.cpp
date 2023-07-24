/*
 * FileObject.cpp
 *
 *  Created on: 16.11.2022
 *      Author: tsa
 */



#include <iostream>
#include <fstream>
#include <cstdint>
#include <filesystem>
#include "FileObject/FileObject.h"

namespace fs = std::filesystem;


FileObject::FileObject(string folder, string filename, string ext, uint32_t filesize, uint16_t numberofFile):
	m_folder(folder), m_fileName(filename), m_curFileIndex(0),
	m_extension(ext), m_maxFileSize(filesize*1024), m_numberOfFile(numberofFile), m_curFileSize(0){

	if(numberofFile > 1) {
		m_circular_buffer = new CircularBuffer(m_maxFileSize, numberofFile);
		m_isRingBuffer = true;
	}
}

FileObject::~FileObject(){
	//falls diese FileObject ein Ringspeicher ist dann speicher freigeben
	if(m_circular_buffer != nullptr)
		delete m_circular_buffer;
}

void FileObject::setCurrentFileSize(uint32_t currSize){
	this->m_curFileSize = currSize;
}
void FileObject::init(){
	auto m_file = new FileObject("Logs",
								"log",
								"txt",
								1 /* 1KB */,
								4);

}

void FileObject::updateFileObject(){
	if(m_circular_buffer != nullptr){
		m_circular_buffer->updateWritePosition();
		m_curFileIndex++;
		m_curFileSize = 0;
		if(m_curFileIndex == 0){
			m_curFileIndex = 1;
		}
	}
}

/*******************************************************************************************
 * Diese Routine liefert zurück, der Ordnername wo die Dateien gespeichert werden sollen.
 *******************************************************************************************/
string FileObject::getFolderName(){
	return this->m_folder;
}

/*******************************************************************************************
 * Diese Routine liefert der Dateiname zurück.
 *******************************************************************************************/
string FileObject::getFileName(){
	if(m_circular_buffer != nullptr){
		if(m_curFileIndex == 0){
			return m_fileName + "_1." + m_extension;
		}else{
			return m_fileName + "_" + to_string(m_curFileIndex) + "." + m_extension;
		}
	}else{
		return this->m_fileName + "." + m_extension;
	}
}

/*******************************************************************************************
 * Diese Routine liefert die Datei-Endung zurück (.txt, .log, .bin ...).
 *******************************************************************************************/
string FileObject::getFileExtension(){
	return this->m_extension;
}

/*******************************************************************************************
 * Diese Routine liefert die maximale Dateigröße zurück.
 *******************************************************************************************/
uint32_t FileObject::getMaxFileSize(){
	return this->m_maxFileSize;
}

/*******************************************************************************************
 * Diese Routine liefert die aktuelle Datei-Index zurück.
 *******************************************************************************************/
uint32_t FileObject::getCurrentFileIndex(){
	return this->m_curFileIndex;
}

/**********************************************************************
 * Diese Routine liefert der Index neueste gespeicherte Datei zurück
 *********************************************************************/
int FileObject::getNewFileId(){

	string p = ROOT_DIR + m_folder;

	//auto writePosFile = p + "\\DO_NOT_TOUCH.txt";
	//const fs::path dirWritePosFile{writePosFile};

	//Liste alle Dateien im Ordner holen TODO
	fs::directory_iterator fileList = fs::directory_iterator(p);
	int fileIndex = 0;
	int prevFileIndex = 0;
	int higherIndex = 0;
	bool firstLoop = true;

	for(const auto& file: fileList){
		//Pfad der gefundene Datei ermitteln
		auto filePath = file.path().string(); //TODO

		//Datei position ermitteln (aus dem Pfad extrahieren
		auto startPosId = filePath.find(m_fileName);
		if(startPosId != string::npos){
			startPosId += m_fileName.size() + 1;
			auto endPosId = filePath.find("." + m_extension);
			if(endPosId != string::npos){
				auto Len = endPosId - startPosId;
				auto fileIndexStr = filePath.substr(startPosId, Len);
				prevFileIndex = fileIndex;
				fileIndex = stoi(fileIndexStr);
				int diff = fileIndex - prevFileIndex;

				if(firstLoop){
					higherIndex = fileIndex;
					firstLoop = false;
				}
				if(prevFileIndex == 0){
					continue;
				}

				if(diff < -1){
					return prevFileIndex;
				}else if(diff > 1){
					return fileIndex;
				}
				higherIndex = fileIndex > higherIndex?fileIndex:higherIndex;
/*
				if(count == m_circular_buffer->getMaxCount()){
					return fileIndex;
				}
*/
			}
		}
	}

	return higherIndex;

}

/**********************************************************************
 * Diese Routine liefert der Index älteste gespeicherte Datei zurück
 *********************************************************************/
int FileObject::getOldFileId(){
	string p = ROOT_DIR + m_folder;

	//auto writePosFile = p + "\\DO_NOT_TOUCH.txt";
	//const fs::path dirWritePosFile{writePosFile};

	//Liste alle Dateien im Ordner holen TODO
	fs::directory_iterator fileList = fs::directory_iterator(p);
	int fileIndex = 0;
	int prevFileIndex = 0;
	int lowerIndex = 0;
	bool firstLoop = true;

	for(const auto& file: fileList){
		//Pfad der gefundene Datei ermitteln
		auto filePath = file.path().string(); //TODO

		//Datei position ermitteln (aus dem Pfad extrahieren
		auto startPosId = filePath.find(m_fileName);
		if(startPosId != string::npos){
			startPosId += m_fileName.size() + 1;
			auto endPosId = filePath.find("." + m_extension);
			if(endPosId != string::npos){
				auto Len = endPosId - startPosId;
				auto fileIndexStr = filePath.substr(startPosId, Len);
				prevFileIndex = fileIndex;
				fileIndex = stoi(fileIndexStr);
				int diff = fileIndex - prevFileIndex;

				if(firstLoop){
					lowerIndex = fileIndex;
					firstLoop = false;
				}
				if(prevFileIndex == 0){
					continue;
				}

				if(diff < -1){
					return fileIndex;
				}else if(diff > 1){
					return prevFileIndex;
				}
				lowerIndex = fileIndex < lowerIndex?fileIndex:lowerIndex;
			}
		}
	}

	return lowerIndex;
}

/***************************************************************
 * Diese routine erzeugt eine neue Ordner
 **************************************************************/
void FileObject:: createFolder(){

}

/***************************************************************
 * Diese routine erzeugt eine neue Datei
 **************************************************************/
void FileObject::createFile(){
	string filepath = "";
	if(m_circular_buffer != nullptr){
		filepath= ROOT_DIR + m_folder + "\\" +
			m_fileName + "_" + to_string(m_curFileIndex) + "." + m_extension;
	}else{
		filepath= ROOT_DIR + m_folder + "\\" +
			m_fileName + "." + m_extension;
	}
	//funktion von MemoryManager um Datei zu erstellen aufrufen.
}

/***************************************************************
 * Diese routine löscht eine Datei im Festspeicher
 **************************************************************/
void FileObject::deleteFile(string filepath){

}
/***************************************************************
 * Diese Rroutine prüft ob eine Datei existiert
 * @return: bool, true wenn die datei existiert sonst false
 **************************************************************/
bool FileObject::isFileExist(string filepath){

	fs::path file{filepath};
	if(fs::exists(file)){
		return true;
	}
	return false;
}

/***************************************************************
 * Diese Routine prüft ob einen Ordner existiert
 * @return: bool, true wenn der Ordner existiert sonst false
 **************************************************************/
bool FileObject::isFolderExist(string folderpath){

	fs::path folder{folderpath};
	if(fs::exists(folder)){
		return true;
	}
	return false;
}

/***********************************************************
 * Diese Routine schreibt string in Datei
 ***********************************************************/
void FileObject::write(string text){
	uint8_t* data = (uint8_t*)(text.c_str());
	uint32_t length = text.length();

	this->write(data, length);
}

/***********************************************************
 * Diese Routine schreibt byte in Datei
 ***********************************************************/
void FileObject::write(uint8_t* data, uint32_t length){
	string path = ROOT_DIR + m_folder;
	string filepath = "";
	static bool firstWrite = true;

	//prüfen ob der Ordner schon existiert
	if(!isFolderExist(path)){
		//falls der Ordner noch nicht existiert dann neue erstellen
		fs::create_directories(path);
	}else{

		if(firstWrite){
			m_curFileIndex = getNewFileId();
			if(m_curFileIndex > 0){
				string file = path + "\\" + m_fileName + "_" +
							to_string(m_curFileIndex) + "." + m_extension;

				m_curFileSize = fs::file_size(file);
				//Liste alle Dateien im Ordner holen TODO
				fs::directory_iterator fileList = fs::directory_iterator(path);
				uint32_t count = 0;

				if(m_circular_buffer != nullptr){
					for(const auto& file: fileList){
						count++;
						m_circular_buffer->updateWritePosition();
						(void)file;
					}
				}
			}
			firstWrite = false;
		}
	}



	//prüfen ob es sich um einen Ringspeicher handeln
	if(m_circular_buffer != nullptr){

		//Dateien werden in Ring gespeichert
		if(m_curFileIndex == 0){
			m_curFileIndex++;
		}
		filepath = path + "\\" + m_fileName + "_" +
							to_string(m_curFileIndex) + "." + m_extension;

		ofstream oFile;

		if((m_curFileSize + length) > m_maxFileSize){

			updateFileObject();

			filepath = path + "\\" + m_fileName + "_" +
								to_string(m_curFileIndex) + "." + m_extension;

			if(m_circular_buffer->getCircularBufferState()){
				//Der ring ist voll. Älteste Eintrag löschen.
				int id = this->getOldFileId();

				if(id > 0){
					string rf = path + "\\" + m_fileName + "_" +
								to_string(id) + "." + m_extension;

					fs::remove(rf);
				}
			}
		}

		if(!isFileExist(filepath)){
			//fs::create_directories(filepath);
			//ein neue Datei wir erstellt falls es noch nicht gibt
			oFile.open(filepath);
			oFile.write((char*)data, length);
			oFile.close();

		}else{

			oFile.open(filepath, ios::app);
			oFile.write((char*)data, length);
			oFile.close();
		}

		m_curFileSize = fs::file_size(filepath);

	}else{
		//nur eine Datei wird gespeichert. Kein Ringspeicher
		filepath = path + "\\" + m_fileName + "." + m_extension;

		//datei erzeugen falls es noch nicht existiert
		if(!isFileExist(filepath)){
			fs::create_directories(filepath);
		}

		//Es wird in der Datei geschrieben
		ofstream oFile;
		oFile.open(filepath);
		oFile.write((char*)data, length);
		oFile.close();

	}

}
