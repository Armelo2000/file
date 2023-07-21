/*
 * FileObject.cpp
 *
 *  Created on: 16.11.2022
 *      Author: tsa
 */


//#include "GPFCommon/Infrastructure/Memory/FileObject.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <filesystem>
//#include <GPFCommon/Infrastructure/Memory/MemoryNode.h>
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
	if(m_file != nullptr)
	m_file->m_log = "Das ist nur ein Test\n";
}

void FileObject::updateFileObject(){
	if(m_circular_buffer != nullptr){

		if(this->m_curFileSize >= this->m_maxFileSize){
			m_circular_buffer->updateWritePosition();
			this->m_curFileSize = 0;
			if(this->m_curFileIndex == MAX_INDEX){
				this->m_curFileIndex = 1;
			}else{
				this->m_curFileIndex++;
			}
		}
	}
}

/**************************************************************************************
 * Diese Methode schreibt im Datei
 * @param: file, das ist das Objekt mit der Datei wo es geschrieben werden soll.
 *************************************************************************************/
void FileObject::save(FileObject* file){

	if(file != nullptr){
		auto p = "C:\\Users\\tsa\\Downloads\\" + file->m_folder;
		auto f = p + "\\" + file->m_fileName + "_" + to_string(file->m_index) + file->m_extension;
		const fs::path dir{p};
		ofstream oFile;
		if(fs::exists(dir)){				// TODO FileSystem : replace the function exists (check if a Path exist) With HAL function
			const fs::path dirFile{f};
			if(fs::exists(dirFile)){		// TODO FileSystem : replace the function exists (check if the file exist) with HAL function
				//Check the size;
				auto fileSizeBefore = fs::file_size(f);
				if(fs::file_size(f) < file->m_maxFileSize){	// TODO FileSystem : replace the function file_size (get the file size) with HAL function
					oFile.open(f, ios::app);	// TODO FileSystem : replace the function open (open file and append) with HAL function
					// write to file
					oFile << file->m_log;		// TODO FileSystem : replace the write function (write to file) with HAL function
					auto fileSizeAfter = fs::file_size(f);
					auto fileSizeMore = fileSizeBefore - fileSizeAfter;

					oFile.close();				// TODO FileSystem : replace the function close (close the file) with HAL function
				}else{
					//size is exceeded
					if(file->m_isRingBuffer){
						if(file->m_index < file->m_numberOfFile){
							file->m_index++;
							f = p + "\\" + file->m_fileName + "_" + to_string(file->m_index) + file->m_extension;
							oFile.open(f);		// TODO FileSystem : replace the function open (open: create new file if not exist) with HAL function
							// write to file
							oFile << file->m_log;	// TODO FileSystem : replace the write function (write to file) with HAL function

							oFile.close();			// TODO FileSystem : replace the function close (close the file) with HAL function

						}else{
							//override or delete the last created file
							uint32_t lastIndex = file->m_index - file->m_numberOfFile + 1;
							file->m_index++;
							f = p + "\\" + file->m_fileName + "_" + to_string(lastIndex) + file->m_extension;
							fs::remove(f);  // TODO FileSystem : replace the function remove (delete the file) with HAL function
							f = p + "\\" + file->m_fileName + "_" + to_string(file->m_index) + file->m_extension;
							oFile.open(f);		// TODO FileSystem : replace the function open (open: create new file if not exist) with HAL function
							// write to file
							oFile << file->m_log;	// TODO FileSystem : replace the write function (write to file) with HAL function

							oFile.close();			// TODO FileSystem : replace the function close (close the file) with HAL function

						}
					}
				}
			}else{
				//file do not exist
				//create and write to it
				oFile.open(f);			// TODO FileSystem : replace the function open (open: create new file if not exist) with HAL function
				// write to file
				oFile << file->m_log;	// TODO FileSystem : replace the write function (write to file) with HAL function

				oFile.close();		// TODO FileSystem : replace the function close (close the file) with HAL function
			}



		}else{
			fs::create_directories(dir);

			oFile.open(f);			// TODO FileSystem : replace the function open (open: create new file if not exist) with HAL function
			// write to file
			oFile << file->m_log;	// TODO FileSystem : replace the write function (write to file) with HAL function

			oFile.close();			// TODO FileSystem : replace the function close (close the file) with HAL function
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
	return this->m_fileName;
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

				if(diff > 1){
					return prevFileIndex;
				}

			}
		}
	}

	return fileIndex;

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

				if(diff > 1){
					return fileIndex;
				}

			}
		}
	}

	return fileIndex;
}

/***************************************************************
 * Diese routine erzeugt eine neue Datei
 **************************************************************/
void FileObject::createFile(){
	string filepath = ROOT_DIR + m_folder + "\\" +
			m_fileName + "_" + to_string(m_curFileIndex) + "." + m_extension;
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


	return false;
}

/***************************************************************
 * Diese Routine prüft ob einen Ordner existiert
 * @return: bool, true wenn der Ordner existiert sonst false
 **************************************************************/
bool FileObject::isFolderExist(string folderpath){

	return false;
}

/***********************************************************
 * Diese Routine schreibt in Datei
 ***********************************************************/
void FileObject::write(uint8_t* data, uint32_t length){
	string path = ROOT_DIR + m_folder;
	string filepath = "";

	if(!isFolderExist(path)){
		//falls der Ordner noch nicht existiert dann neue erstellen
	}
	if((m_circular_buffer != nullptr) && (m_curFileIndex == 0)) {
		m_curFileIndex = 1;
	}

	if(m_circular_buffer != nullptr){
		filepath = path + "\\" + m_fileName + "_" +
					to_string(m_curFileIndex) + "." + m_extension;
	}else{
		filepath = path + "\\" + m_fileName + "." + m_extension;
	}


	//prüfen ob filepath schon existiert
	if(isFileExist(filepath)){
		//wenn ja in datei weiter schreiben(Ring) oder überschreiben und der aktuelle datei größe absichern
		if(m_circular_buffer != nullptr){
			//append
		}else{
			//überschreiben
		}
	}
	//wenn nein, prüfen ob es ein ringspeicher ist
	// - Ring => es soll geprüft werden ob der ring voll ist
	//    + Ring voll - alte datei löschen neu erstellen und schreiben
	//    + Ring nicht voll - keine datei löschen sondern aktuelle weiter schreiben

}
