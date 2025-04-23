#include "Credential.hpp"
#include <algorithm>
#include <cctype>

std::string toLowerCase(const std::string& input) {
	std::string result = input;
	std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
	return result;
}

enum class GeneralCredentialKeys { Title, Username, Password, Wallet, Email };

class GeneralCredential : public Credential {
public:
	GeneralCredential() : Credential(Credential::Type::General), Username(""), Password("") {}
	GeneralCredential(int id, const char* title, const char* name, const char* password) : Credential(Credential::Type::General, id, title), Username(name), Password(password) {}
	GeneralCredential(int id, std::string title, std::string name, std::string password) : Credential(Credential::Type::General, id, title), Username(name), Password(password) {}
	GeneralCredential(const char* title, const char* name, const char* password) : Credential(Credential::Type::General, title), Username(name), Password(password) {}
	GeneralCredential(std::string title, std::string name, std::string password) : Credential(Credential::Type::General, title), Username(name), Password(password) {}
	GeneralCredential(int id, const char* title, const char* name, const char* password, const char* wallet, const char* email) : Credential(Credential::Type::General, id, title), Username(name), Password(password), Wallet(wallet), Email(email) {}
	GeneralCredential(const char* title, const char* name, const char* password, const char* wallet, const char* email) : Credential(Credential::Type::General, title), Username(name), Password(password), Wallet(wallet), Email(email) {}
	GeneralCredential(int id, std::string title, std::string name, std::string password, std::string wallet, std::string email) : Credential(Credential::Type::General, id, title), Username(name), Password(password), Wallet(wallet), Email(email) {}
	GeneralCredential(std::string title, std::string name, std::string password, std::string wallet, std::string email) : Credential(Credential::Type::General, title), Username(name), Password(password), Wallet(wallet), Email(email) {}

	void print() const override {
		std::cout << "Username: " << Username << "\tPassword: " << Password << "\tID: " << ID << "\tTitle: " << Title << std::endl;
	}

	float Search(std::string searchText) const override {
		searchText = toLowerCase(searchText);
		std::string t = toLowerCase(this->Title);
		std::string u = toLowerCase(this->Username);
		std::string w = toLowerCase(this->Wallet);
		std::string e = toLowerCase(this->Email);

		if (
			t.find(searchText) == std::string::npos && u.find(searchText) == std::string::npos
			&&
			w.find(searchText) == std::string::npos && e.find(searchText) == std::string::npos
			) {
			return 0;
		}

		return std::max(
			std::max(
				similarity(t, searchText),
				similarity(u, searchText)
				),
			std::max(
				similarity(w, searchText),
				similarity(e, searchText)
				)
		);
	}

	static std::string SelectALLSQL() {
		return "SELECT ID, Title, Username, Password, Wallet, Email FROM GeneralCredential";
	}

	static std::string InsertSQL() {
		return "INSERT INTO GeneralCredential (Title, Username, Password, Wallet, Email) VALUES (?, ?, ?, ?, ?);";
	}

	static std::string UpdateSQL() {
		return "UPDATE GeneralCredential SET Title = ?, Username = ?, Password = ?, Wallet = ?, Email = ? WHERE ID = ?;";
	}

	static std::string CreateTableSQL() {
		return std::string("CREATE TABLE IF NOT EXISTS GeneralCredential (ID INTEGER PRIMARY KEY AUTOINCREMENT, Title TEXT, Username TEXT, Password TEXT, Wallet TEXT, Email TEXT);");
	}

	static std::string DeleteSQL(int id) {
		return "DELETE FROM GeneralCredential WHERE ID = " + std::to_string(id);
	}

	static GeneralCredential FromRAW(sqlite3_stmt* stmt) {
		GeneralCredential gc;
		gc.ID = sqlite3_column_int(stmt, 0);
		gc.Title = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
		gc.Username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
		gc.Password = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
		gc.Wallet = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
		gc.Email = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));

		return gc;
	}

	void BindToSTMT(sqlite3_stmt* stmt, bool withID = false) const  {
		int index = 1;
		if(withID) sqlite3_bind_int(stmt, index++, ID);
		sqlite3_bind_text(stmt, index++, (this->Title).c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, index++, (this->Username).c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, index++, (this->Password).c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, index++, (this->Wallet).c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, index++, (this->Email).c_str(), -1, SQLITE_STATIC);
	}

	void Set(GeneralCredentialKeys which, std::string value){
		switch (which)
		{
		case GeneralCredentialKeys::Title:
			this->Title = value;
			break;
		case GeneralCredentialKeys::Username:
			this->Username = value;
			break;
		case GeneralCredentialKeys::Password:
			this->Password = value;
			break;
		case GeneralCredentialKeys::Wallet:
			this->Wallet = value;
			break;
		case GeneralCredentialKeys::Email:
			this->Email = value;
			break;
		default:
			break;
		}
	}

	std::string Get(GeneralCredentialKeys which) {
		switch (which)
		{
		case GeneralCredentialKeys::Title:
			return this->Title;
			break;
		case GeneralCredentialKeys::Username:
			return this->Username;
			break;
		case GeneralCredentialKeys::Password:
			return this->Password;
			break;
		case GeneralCredentialKeys::Wallet:
			return this->Wallet;
			break;
		case GeneralCredentialKeys::Email:
			return this->Email;
			break;
		default:
			return "";
			break;
		}
	}

	std::string Username;
	std::string Password;
	std::string Wallet;
	std::string Email;
private:
};