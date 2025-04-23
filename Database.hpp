#include <string>
#include <filesystem>
#include <sqlite3.h>
#include <iostream>


class Database {
public:

	Database(std::string path)
	{
		this->pathLocation = path;

		if (std::filesystem::is_directory(this->pathLocation)) {
			this->pathLocation /= "database.db";
		}

		int rc = sqlite3_open((this->pathLocation).string().c_str(), &this->database);

		if (rc != SQLITE_OK) {
			std::cout << sqlite3_errmsg(database) << std::endl;
			sqlite3_close(this->database);
			this->database = nullptr;
		}

	}

	void RunSQL(std::string sql) {
		char* zErrMsg = 0;
		int rc = sqlite3_exec(this->database, sql.c_str(), NULL, NULL, &zErrMsg);
		
		if (rc != SQLITE_OK) {
			std::cout << sqlite3_errmsg(database) << std::endl;
			std::cout << zErrMsg << std::endl;
		}
		delete zErrMsg;
	}

	~Database() {
		if (database != nullptr) {
			sqlite3_close(database);
			this->database = nullptr;
		}
	}

	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	std::vector<GeneralCredential> GetAll() const {
		std::vector<GeneralCredential> credentials;
		sqlite3_stmt* stmt = nullptr;
		std::string sql = GeneralCredential::SelectALLSQL();

		if (sqlite3_prepare_v3(this->database, sql.c_str(), -1, 0, &stmt, nullptr) != SQLITE_OK) {
			std::cerr << "Error " << sqlite3_errmsg(this->database) << std::endl;
			return credentials;
		}

		while(sqlite3_step(stmt) == SQLITE_ROW) {
			credentials.push_back(GeneralCredential::FromRAW(stmt));
		}
		sqlite3_finalize(stmt);
		return credentials;
	}

	bool Insert(const std::vector<GeneralCredential>& items) {
		if (items.empty()) return true;

		sqlite3_exec(this->database, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

		const std::string& sql = GeneralCredential::InsertSQL();
		sqlite3_stmt* stmt = nullptr;

		if (sqlite3_prepare_v3(this->database, sql.c_str(), -1, 0, &stmt, nullptr) != SQLITE_OK) {
			std::cerr << "Error " << sqlite3_errmsg(this->database) << std::endl;
			sqlite3_exec(this->database, "ROLLBACK;", nullptr, nullptr, nullptr);
			return false;
		}

		for (const GeneralCredential& item : items) {
			item.BindToSTMT(stmt);
			if (sqlite3_step(stmt) != SQLITE_DONE) {
				std::cerr << "Error " << sqlite3_errmsg(this->database) << std::endl;
				sqlite3_finalize(stmt);
				sqlite3_exec(this->database, "ROLLBACK;", nullptr, nullptr, nullptr);
				return false;
			}
			sqlite3_reset(stmt);
		}
		sqlite3_finalize(stmt);

		sqlite3_exec(this->database, "COMMIT;", nullptr, nullptr, nullptr);
		return true;
	}

	bool Delete(const GeneralCredential& item) {
		return this->Delete(item.ID);
	}

	bool Delete(int ID) {
		std::string sql = GeneralCredential::DeleteSQL(ID);
		int r = sqlite3_exec(this->database, sql.c_str(), nullptr, nullptr, nullptr);
		if (r != SQLITE_OK) {
			std::cerr << "Error " << sqlite3_errmsg(this->database) << std::endl;
			return false;
		}
		return true;
	}

	bool Update(const GeneralCredential& item) {

		item.print();

		std::string sql = GeneralCredential::UpdateSQL();
		sqlite3_stmt* stmt;

		if(sqlite3_prepare_v3(this->database, sql.c_str(), -1, 0, &stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "Error " << sqlite3_errmsg(this->database) << std::endl;
			return false;
		}

		sqlite3_bind_text(stmt, 1, item.Title.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, item.Username.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, item.Password.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 4, item.Wallet.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 5, item.Email.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 6, item.ID);

		int r = sqlite3_step(stmt);

		if (r != SQLITE_DONE) {
			std::cerr << "Error " << sqlite3_errmsg(this->database) << std::endl;
			sqlite3_finalize(stmt);
			return false;
		}

		sqlite3_finalize(stmt);
		return true;
	}

private:
	std::filesystem::path pathLocation = "";
	sqlite3* database = nullptr;
};