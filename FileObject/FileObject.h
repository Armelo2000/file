/********************************************************************************************************************

 ********************************************************************************************************************/

#ifndef GPFCOMMON_INFRASTRUCTURE_MEMORY_FILEOBJECT_H_
#define GPFCOMMON_INFRASTRUCTURE_MEMORY_FILEOBJECT_H_

#include "Common.h"
#include <string>
#include "RingBuffer/CircularBuffer.h"
#include <memory>

class MemoryNode;

using namespace std;

class FileObject{

	//friend class MemoryManagementCloManager;
private:
	/********************************************************
	 * das ist die maximale Datei-index
	 *******************************************************/
	static const uint32_t MAX_INDEX		= 9;

	/******************************************************************
	 * Das ist der globale pfad zu alle Dateien
	 ******************************************************************/
	inline static const string ROOT_DIR = "C:\\Users\\melah\\Desktop\\FileObject\\";

	/*************************************************************************************************
	 * Diese Variable ist der Präfix von Dateipfad. Das heißt alles Zeichen vor dem Foldername
	 * Es könnte zum Beispiel so aussehen
	 *
	 * 	- "C\\Users\\tsa\\Desktop\\Foldername\\Dateiname.txt"
	 *
	 * 	hier könnte der Präfix so sein: "C\\Users\\tsa\\Desktop\\"
	 *
	 * 	//TODO diese kann erst festgelegt oder dynamisch erstellt wenn Filesystem implementiert ist
	 ************************************************************************************************/
	inline static const string PREFIX	{""};

	/*******************************************************************************
	 * Diese Member besitzt der Name vom Ordner wo die Dateien gespeichert werden.
	 *******************************************************************************/
	string m_folder;

	/*******************************************************************************
	 * Diese Member besitzt der Dateiname, die gespeichert werden soll.
	 * Achtung:
 	 *	 	 	 es wird keine Dateiendung in der Dateiname eingetrgen.
 	 *	 	 	 Die Dateiendung wird separat betrachtet. Das ermöglicht die Index
 	 *	 	 	 beim jede Datei vor die Dateiendung zu schreiben.
	 *******************************************************************************/
	string m_fileName;

	/******************************************************************************
	 * Hier wird die Dateiendung gespeichert. Die Dateiendung könnte eine der
	 * folgende sein: .txt, .bin, .log und mehr
	 *
	 * Achtung:
	 * 		der Punkt vor hier gespeichert aber beim erstellen von FileObject
	 * 		nicht eingegeben.
	 *****************************************************************************/
	string m_extension;

	/*********************************************************************************
	 * Dies ist die maximale Dateigröße. Falls es sich um einem Ringspeicher handelt
	 * soll jeder Datei im Ring diese maximale Dateigröße haben.
	 *
	 * Achtung:
	 * 			Die Dateigröße wird in KB (Kilo Byte) eingegeben.
	 *********************************************************************************/
	uint32_t m_maxFileSize;		// in KB

	/*********************************************************************************
	 * Dies ist die aktuelle Dateigröße. Diese Dateigröße darf nicht die maximale
	 * Dateigröße überschreiten.
	 *
	 * Achtung:
	 * 			Die Dateigröße wird in KB (Kilo Byte) eingegeben.
	 *********************************************************************************/
	uint32_t m_curFileSize;		// in KB

	/*********************************************************************************
	 * Dies ist die aktuelle Datei-index. Damit kann die aktuelle Dateiname ermitteln
	 * werden
	 *
	 *********************************************************************************/
	uint32_t m_curFileIndex;		// in KB

	/**********************************************************************************
	 * Objekt für das Ringspeicher
	 */
	CircularBuffer* m_circular_buffer;
	//shared_ptr<CircularBuffer> m_circular;

	/********************************************************************************
	 * Diese Routine setzt die aktuelle Dateigröße.
	 *
	 * @param: currSize, das ist die aktuelle Dateigröße die gesetzt werden soll
	 */
	void setCurrentFileSize(uint32_t currSize);

	/***************************************************************
	 * Diese routine erzeugt eine neue Datei
	 **************************************************************/
	void createFile();

	/***************************************************************
	 * Diese Routine löscht eine Datei im Festspeicher
	 **************************************************************/
	void deleteFile(string filepath);

	/***************************************************************
	 * Diese Rroutine prüft ob eine Datei existiert
	 * @return: bool, true wenn die datei existiert sonst false
	 **************************************************************/
	bool isFileExist(string filepath);

	/***************************************************************
	 * Diese Routine prüft ob einen Ordner existiert
	 * @return: bool, true wenn der Ordner existiert sonst false
	 **************************************************************/
	bool isFolderExist(string folderpath);

public:

	FileObject(string folder, string filename, string ext, uint32_t filesize, uint16_t numberofFile);
	~FileObject();

	/**************************************************************************************
	 * Diese Methode schreibt im Datei
	 * @param: file, das ist das Objekt mit der Datei wo es geschrieben werden soll.
	 *************************************************************************************/
	void save(FileObject* file);

	/*******************************************************************************************
	 * Diese Routine liefert zurück, der Ordnername wo die Dateien gespeichert werden sollen.
	 *******************************************************************************************/
	string getFolderName();

	/*******************************************************************************************
	 * Diese Routine liefert der Dateiname zurück.
	 *******************************************************************************************/
	string getFileName();

	/*******************************************************************************************
	 * Diese Routine liefert die Datei-Endung zurück (.txt, .log, .bin ...).
	 *******************************************************************************************/
	string getFileExtension();

	/*******************************************************************************************
	 * Diese Routine liefert die maximale Dateigröße zurück.
	 *******************************************************************************************/
	uint32_t getMaxFileSize();

	/*******************************************************************************************
	 * Diese Routine liefert die aktuelle Datei-Index zurück.
	 *******************************************************************************************/
	uint32_t getCurrentFileIndex();

	/**********************************************************************
	 * Diese Routine liefert der Index neueste gespeicherte Datei zurück
	 *********************************************************************/
	int getNewFileId();

	/**********************************************************************
	 * Diese Routine liefert der Index älteste gespeicherte Datei zurück
	 *********************************************************************/
	int getOldFileId();

	/*******************************************************************************************
	 * TODO Diese Routine ist ein Test um die Erstellung vom Objekt FileObject zu prüfen.
	 *******************************************************************************************/
	void init();

	void updateFileObject();

	/***********************************************************
	 * Diese Routine schreibt in Datei
	 ***********************************************************/
	void write(uint8_t* data, uint32_t length);


	/*******************************************************************************************
	 * Diese Variable gibt an der Index von Datei im Ordner. Es soll immer inkrementiert werden
	 * wenn einen neue Datei erstellt wird. Diese kommt viel vor beim Ringspeichern von Dateien
	 *******************************************************************************************/
	uint32_t m_index{1};

	/*******************************************************************************************
	 * Diese Variable beinhalte der string, der im Datei geschrieben werden soll.
	 *******************************************************************************************/
	string m_log;			//data to log

//private:

	/****************************************************************************************
	 * Diese Member ist die Anzahl von Dateien im Ringspeicher
	 ****************************************************************************************/
	uint16_t m_numberOfFile;

	/****************************************************************************************
	 * Diese Member gibt an, ob die Datei sich im Ringspeicher befindet.
	 * 		- Es handelt sich um einem Ringspeicher wenn der true ist ansonst false.
	 ****************************************************************************************/
	bool m_isRingBuffer	{false};

};


#endif /* GPFCOMMON_INFRASTRUCTURE_MEMORY_FILEOBJECT_H_ */
