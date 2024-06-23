#include "DatabaseAccess.h"

bool callbackCalled = false;

DatabaseAccess::DatabaseAccess()
	:_db(nullptr)
{
}

DatabaseAccess::~DatabaseAccess()
{
}

void DatabaseAccess::createUser(User& user)
{
	
	std::string sqlQuery = "INSERT INTO USERS(NAME) VALUES( '" + user.getName() + "')";
	bool result;

	sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);
}

void DatabaseAccess::deleteUser(const User& user)
{
	
	std::string sqlQuery = "DELETE FROM USERS WHERE ID = " + std::to_string(user.getId()) + " AND NAME = '" + user.getName() + "'";
	bool result;

	sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);
}

void DatabaseAccess::printUsers()
{
	std::list<User> userList;
	std::string sqlQuery = "SELECT * FROM USERS";
	char* errMessage = nullptr;
	bool res;

	res = sqlite3_exec(_db, sqlQuery.c_str(), getUsersCallback, (void*)&userList, &errMessage);

	std::cout << "Users list:" << std::endl;
	std::cout << "-----------" << std::endl;
	for (const auto& user : userList)
		std::cout << user << std::endl;
}

User DatabaseAccess::getUser(int userId)
{
	User user(0, "");
	std::string sqlQuery = "SELECT * FROM USERS WHERE ID = " + std::to_string(userId);
	char* errMessage = nullptr;
	bool res;

	callbackCalled = false;
	res = sqlite3_exec(_db, sqlQuery.c_str(), getOneUserCallback, (void*)&user, &errMessage);


	if (!callbackCalled)
	{
		callbackCalled = false;
		throw MyException("No user with id " + std::to_string(userId) + " exists");
	}
	

	return user;
}

bool DatabaseAccess::doesUserExists(int userId)
{
	//very similar to getUser, utilize that the callback isn't called
	User user(0, "");
	std::string sqlQuery = "SELECT * FROM USERS WHERE ID = " + std::to_string(userId);
	char* errMessage = nullptr;
	bool res;

	callbackCalled = false;
	res = sqlite3_exec(_db, sqlQuery.c_str(), getOneUserCallback, (void*)&user, &errMessage);

	if (!callbackCalled)
	{
		callbackCalled = false;
		return false;
	}

	return true;
}

void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	//delete the tags and the pictures
	std::string sqlQuery = "DELETE FROM TAGS WHERE PICTURE_ID IN(SELECT ID FROM PICTURES WHERE ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "'))";
	sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);

	//delete all pictures of album
	sqlQuery = "DELETE FROM PICTURES WHERE ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "')";
	sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);

	sqlQuery = "DELETE FROM ALBUMS WHERE USER_ID = " + std::to_string(userId) + " AND NAME = '" + albumName + "'";
	sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);
}

const std::list<Album> DatabaseAccess::getAlbums()
{
	std::string sqlQuery = "SELECT NAME FROM ALBUMS";
	std::list<std::string> albumNames;
	std::list<Album> albumList;
	char* errMessage = nullptr;
	bool res;

	res = sqlite3_exec(_db, sqlQuery.c_str(), getAlbumsNamesCallback, (void*) &albumNames, &errMessage);

	for (const auto& name : albumNames)
		albumList.push_back(openAlbum(name));

	return albumList;
}

const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user)
{
	std::string sqlQuery = "SELECT * FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId());
	std::list<Album> lst;
	char* errMessage = nullptr;
	bool res;

	res = sqlite3_exec(_db, sqlQuery.c_str(), getAlbumsCallback, (void*)&lst, &errMessage);

	return lst;
}

void DatabaseAccess::createAlbum(const Album& album)
{
	
	std::string sqlQuery = "INSERT INTO ALBUMS(NAME, CREATION_TIME, USER_ID) VALUES('" + album.getName() +  "', '" + album.getCreationDate() + "', " + std::to_string(album.getOwnerId()) +  ")";
	bool result;

	sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);
}

bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	
	std::string sqlQuery = "SELECT COUNT(*) FROM ALBUMS WHERE NAME = '" + albumName + "' AND USER_ID =  " + std::to_string(userId);
	int count;

	sqlite3_exec(_db, sqlQuery.c_str(), getCountCallback, &count, nullptr);

	return count > 0;
}

Album DatabaseAccess::openAlbum(const std::string& albumName)
{
	std::string sqlQuery = "SELECT * FROM ALBUMS WHERE NAME = '" + albumName + "'";
	char* errMessage = nullptr;
	bool res;
	Album album;

	callbackCalled = false;
	res = sqlite3_exec(_db, sqlQuery.c_str(), getOneAlbumCallback, (void*)&album, &errMessage);


	if (!callbackCalled)
	{
		callbackCalled = false;
		throw MyException("No album with name " + albumName + " exists");
	}
	
	std::list<Picture> pictures;
	//now get the albums pictures
	sqlQuery = "SELECT PICTURES.* FROM PICTURES JOIN ALBUMS ON PICTURES.ALBUM_ID = ALBUMS.ID WHERE ALBUMS.NAME = '" + albumName + "'";
	res = sqlite3_exec(_db, sqlQuery.c_str(), addPictureCallback, (void*)&pictures, &errMessage);

	// Iterate through each picture in the album
	for (auto& picture : pictures)
	{
		int id = -1; 
		callbackCalled = false;
		std::string sqlQuery = "SELECT USER_ID FROM TAGS WHERE PICTURE_ID = " + std::to_string(picture.getId());
		res = sqlite3_exec(_db, sqlQuery.c_str(), getPictureTagCallback, &picture, &errMessage);

		album.addPicture(picture);
	}

	return album;
}

void DatabaseAccess::closeAlbum(Album& pAlbum)
{
}

void DatabaseAccess::printAlbums()
{
	std::list<Album> albumList = getAlbums();

	if(albumList.empty())
		throw MyException("There are no existing albums.");

	std::cout << "Album list:" << std::endl;
	std::cout << "-----------" << std::endl;

	for(const auto& album : albumList)
		std::cout << std::setw(5) << "* " << album;
}

void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	std::string sqlQuery = "INSERT INTO TAGS (USER_ID, PICTURE_ID) VALUES (" + std::to_string(userId) + ", (SELECT PICTURES.ID FROM PICTURES INNER JOIN ALBUMS ON PICTURES.ALBUM_ID = ALBUMS.ID WHERE PICTURES.NAME = '" + pictureName + "' AND ALBUMS.NAME = '" + albumName + "'))";
	bool result;

	sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);
}

void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	
	std::string sqlQuery = "DELETE FROM TAGS WHERE TAGS.USER_ID = " + std::to_string(userId) + " AND TAGS.PICTURE_ID = (SELECT PICTURES.ID FROM PICTURES INNER JOIN ALBUMS ON PICTURES.ALBUM_ID = ALBUMS.ID WHERE PICTURES.NAME = '" + pictureName + "' AND ALBUMS.NAME = '" + albumName +  "')";
	bool result;
	
	sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);
}

void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	std::string sqlQuery = "INSERT INTO PICTURES(NAME, LOCATION, CREATION_TIME, ALBUM_ID) VALUES('" + picture.getName() + "', '" + picture.getPath() + "', '" + picture.getCreationDate() + "', (SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "'))";
	bool result;

	sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);
}

void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	//remove all the tags associated with the picture
	std::string sqlQuery = "DELETE FROM TAGS WHERE PICTURE_ID = (SELECT ID FROM PICTURES WHERE NAME = '" + pictureName + "' AND ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "'))";
	sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);

	//remove the picture
	sqlQuery = "DELETE FROM PICTURES WHERE NAME = '" + pictureName + "' AND ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "')";
	sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);
}

int DatabaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	std::string sqlQuery = "SELECT COUNT(*) FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId());
	bool result;
	char* errMessage = nullptr;
	int countOfAlbums = 0;

	result = sqlite3_exec(_db, sqlQuery.c_str(), getCountCallback, &countOfAlbums, &errMessage);


	return countOfAlbums;
}

int DatabaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	std::string sqlQuery = "SELECT COUNT(DISTINCT ALBUMS.ID) AS albums_count FROM ALBUMS JOIN PICTURES ON ALBUMS.ID = PICTURES.ALBUM_ID JOIN TAGS ON PICTURES.ID = TAGS.PICTURE_ID WHERE TAGS.USER_ID = " + std::to_string(user.getId());

	bool result;
	char* errMessage = nullptr;
	int countOfTags = 0;

	result = sqlite3_exec(_db, sqlQuery.c_str(), getCountCallback, &countOfTags, &errMessage);

	return countOfTags;
}

int DatabaseAccess::countTagsOfUser(const User& user)
{
	std::string sqlQuery = "SELECT COUNT(*) FROM TAGS WHERE USER_ID = " + std::to_string(user.getId());

	bool result;
	char* errMessage = nullptr;
	int countOfTags = 0;

	result = sqlite3_exec(_db, sqlQuery.c_str(), getCountCallback, &countOfTags, &errMessage);

	return countOfTags;
}

float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	int albumsTaggedCount = countAlbumsTaggedOfUser(user);

	if (0 == albumsTaggedCount) {
		return 0;
	}

	return static_cast<float>(countTagsOfUser(user)) / albumsTaggedCount;
}

User DatabaseAccess::getTopTaggedUser()
{
	//this sql query groups the tags by userID and then counts, then by oredring you get the most tags
	std::string sqlQuery = "SELECT USER_ID FROM TAGS GROUP BY USER_ID ORDER BY COUNT(USER_ID) DESC LIMIT 1";
	bool result;
	char* errMessage = nullptr;
	int userId = 0;
	result = sqlite3_exec(_db, sqlQuery.c_str(), getCountCallback, &userId, &errMessage);
	
	//i decided to split the sql queries to make it more convenient and readable
	return getUser(userId);
}

Picture DatabaseAccess::getTopTaggedPicture()
{
	//this sql query is almost the same as the getTopTagged user, the change is from USER_ID to PICTURE_ID
	std::string sqlQuery = "SELECT TAGS.PICTURE_ID FROM TAGS GROUP BY TAGS.PICTURE_ID ORDER BY COUNT(TAGS.PICTURE_ID) DESC LIMIT 1";
	bool result;
	char* errMessage = nullptr;
	int pictureId;
	Picture picture(0, "");
	result = sqlite3_exec(_db, sqlQuery.c_str(), getCountCallback, &pictureId, &errMessage);

	sqlQuery = "SELECT * FROM PICTURES WHERE ID = " + std::to_string(pictureId);

	result = sqlite3_exec(_db, sqlQuery.c_str(), getOnePictureCallback, &picture, &errMessage);


	return picture;
}

std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user)
{
	std::list<Picture> pictures;
	std::string sqlQuery = "SELECT * FROM PICTURES WHERE ID IN (SELECT PICTURE_ID FROM TAGS WHERE USER_ID = " + std::to_string(user.getId()) + ")";
	bool result;
	char* errMessage = nullptr;
	result = sqlite3_exec(_db, sqlQuery.c_str(), getPicturesCallback, &pictures, &errMessage);

	// Iterate through each picture and get tags
	for (auto& picture : pictures)
	{
		int id = -1;
		callbackCalled = false;
		sqlQuery = "SELECT USER_ID FROM TAGS WHERE PICTURE_ID = " + std::to_string(picture.getId());
		sqlite3_exec(_db, sqlQuery.c_str(), getPictureTagCallback, &picture, &errMessage);
	}

	return pictures;
}


bool DatabaseAccess::open()
{
	std::ifstream file(DB_NAME);
	if (!file.good())
	{
		file.close();
		//create the database
		sqlite3_open(DB_NAME, &_db);
		std::string sqlQuery = "CREATE TABLE USERS(ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT NOT NULL)";
		sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);

		sqlQuery = "CREATE TABLE ALBUMS(ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT NOT NULL, CREATION_TIME TEXT NOT NULL, USER_ID INTEGER, FOREIGN KEY(USER_ID) REFERENCES  USERS(ID))";
		sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);

		sqlQuery = "CREATE TABLE PICTURES(ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT NOT NULL, LOCATION TEXT NOT NULL, CREATION_TIME TEXT NOT NULL, ALBUM_ID INTEGER, FOREIGN KEY(ALBUM_ID) REFERENCES  ALBUMS(ID))";
		sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);

		sqlQuery = "CREATE TABLE TAGS(ID INTEGER PRIMARY KEY AUTOINCREMENT, PICTURE_ID INTEGER, USER_ID INTEGER, FOREIGN KEY(PICTURE_ID) REFERENCES  PICTURES(ID), FOREIGN KEY(USER_ID) REFERENCES  USERS(ID))";
		sqlite3_exec(_db, sqlQuery.c_str(), nullptr, nullptr, nullptr);
	}
	else
	{
		sqlite3_open(DB_NAME, &_db);
	}

	file.close();
	return true;
}

void DatabaseAccess::close()
{
	sqlite3_close(_db);
}

void DatabaseAccess::clear()
{
}

int getAlbumsCallback(void* data, int argc, char** argv, char** azColName)
{
	Album album;

	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "NAME")
			album.setName(argv[i]);
		else if (std::string(azColName[i]) == "CREATION_DATE")
			album.setCreationDate(argv[i]);
		else if (std::string(azColName[i]) == "USER_ID")
			album.setOwner(atoi(argv[i]));
	}

	((std::list<Album>*)data)->push_back(album);

	return 0;
}

int getOneAlbumCallback(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "NAME")
			((Album*)data)->setName(argv[i]);
		else if (std::string(azColName[i]) == "CREATION_DATE")
			((Album*)data)->setCreationDate(argv[i]);
		else if (std::string(azColName[i]) == "USER_ID")
			((Album*)data)->setOwner(atoi(argv[i]));
	}
	callbackCalled = true;
	return 0;
}

int addPictureCallback(void* data, int argc, char** argv, char** azColName)
{
	Picture picture = Picture(0, "");

	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID")
			picture.setId(atoi(argv[i]));
		else if (std::string(azColName[i]) == "LOCATION")
			picture.setPath(argv[i]);
		else if (std::string(azColName[i]) == "CREATION_DATE")
			picture.setCreationDate(argv[i]);
		else if (std::string(azColName[i]) == "NAME")
			picture.setName(argv[i]);

	}

	((std::list<Picture>*)data)->push_back(picture);

	return 0;
}

int getUsersCallback(void* data, int argc, char** argv, char** azColName)
{
	User user(0, "");

	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID")
			user.setId(atoi(argv[i]));
		else if (std::string(azColName[i]) == "NAME")
			user.setName(std::string(argv[i]));
	}

	((std::list<User>*)data)->push_back(user);

	return 0;
}

int getOneUserCallback(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID")
			((User*)data)->setId(atoi(argv[i]));
		else if (std::string(azColName[i]) == "NAME")
			((User*)data)->setName(argv[i]);
	}
	callbackCalled = true;
	return 0;
}

int getCountCallback(void* data, int argc, char** argv, char** azColName)
{
	//use 0 because its the first value
	*((int*)data) = atoi(argv[FIRST_VALUE]);
	callbackCalled = true;
	return 0;
}

int getAlbumsNamesCallback(void* data, int argc, char** argv, char** azColName)
{
		((std::list<std::string>*)data)->push_back(argv[FIRST_VALUE]);
		return 0;
}

int getOnePictureCallback(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID")
			((Picture*)data)->setId(atoi(argv[i]));
		else if (std::string(azColName[i]) == "NAME")
			((Picture*)data)->setName(argv[i]);
		else if (std::string(azColName[i]) == "LOCATION")
			((Picture*)data)->setPath(argv[i]);
		else if (std::string(azColName[i]) == "CREATION_DATE")
			((Picture*)data)->setCreationDate(argv[i]);
	}
	return 0;
}

int getPictureIdCallback(void* data, int argc, char** argv, char** azColName)
{
	((std::vector<int>*)data)->push_back(atoi(argv[FIRST_VALUE]));
	return 0;
}

int getPicturesCallback(void* data, int argc, char** argv, char** azColName)
{
	Picture picture = Picture(0, "");

	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID")
			picture.setId(atoi(argv[i]));
		else if (std::string(azColName[i]) == "NAME")
			picture.setName(argv[i]);
		else if (std::string(azColName[i]) == "LOCATION")
			picture.setPath(argv[i]);
		else if (std::string(azColName[i]) == "CREATION_DATE")
			picture.setCreationDate(argv[i]);
	}

	((std::list<Picture>*)data)->push_back(picture);

	return 0;
}

int getPictureTagCallback(void* data, int argc, char** argv, char** azColName)
{
	((Picture*)data)->tagUser(atoi(argv[FIRST_VALUE]));

	return 0;
}
