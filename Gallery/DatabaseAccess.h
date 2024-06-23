#pragma once
#include "Lib/sqlite3.h"
#include "IDataAccess.h"
#include "User.h"
#include <fstream>
#include "MyException.h"
#include <map>
#include <vector>

#define DB_NAME "galleryDB.sqlite"
#define FIRST_VALUE 0

class DatabaseAccess : public IDataAccess
{
public:
	DatabaseAccess();
	~DatabaseAccess();

	//user actions
	void createUser(User& user) override;
	void deleteUser(const User& user) override;
	void printUsers() override;
	User getUser(int userId) override;
	bool doesUserExists(int userId) override;

	//album actions
	void deleteAlbum(const std::string& albumName, int userId) override;
	const std::list<Album> getAlbums() override;
	const std::list<Album> getAlbumsOfUser(const User& user) override;
	void createAlbum(const Album& album) override;
	bool doesAlbumExists(const std::string& albumName, int userId) override;
	Album openAlbum(const std::string& albumName) override;
	void closeAlbum(Album& pAlbum) override;
	void printAlbums();

	//picture actions
	void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) override;
	void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) override;

	// user statistics
	int countAlbumsOwnedOfUser(const User& user) override;
	int countAlbumsTaggedOfUser(const User& user) override;
	int countTagsOfUser(const User& user) override;
	float averageTagsPerAlbumOfUser(const User& user) override;

	// queries
	User getTopTaggedUser() override;
	Picture getTopTaggedPicture() override;
	std::list<Picture> getTaggedPicturesOfUser(const User& user) override;

	//basic functions
	bool open() override;
	void close() override;
	void clear() override;

private:
	sqlite3* _db;
};

//Call back functions
int getAlbumsCallback(void* data, int argc, char** argv, char** azColName);//for several albums
int getOneAlbumCallback(void* data, int argc, char** argv, char** azColName);//for one album
int addPictureCallback(void* data, int argc, char** argv, char** azColName);
int getUsersCallback(void* data, int argc, char** argv, char** azColName);//for several users
int getOneUserCallback(void* data, int argc, char** argv, char** azColName);//for one user
int getCountCallback(void* data, int argc, char** argv, char** azColName);
int getAlbumsNamesCallback(void* data, int argc, char** argv, char** azColName);
int getOnePictureCallback(void* data, int argc, char** argv, char** azColName);
int getPictureIdCallback(void* data, int argc, char** argv, char** azColName);
int getPicturesCallback(void* data, int argc, char** argv, char** azColName);
int getPictureTagCallback(void* data, int argc, char** argv, char** azColName);