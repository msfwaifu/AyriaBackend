/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-8-9
    Notes:
        Users are authenticated against the PHPBB forum.
*/

#include <Configuration\All.h>
#include <Services\Servicehandler.h>
#include <json.hpp>
#include <base64.h>
#include <sm4_cl.hpp>
#include <sm3_cl.hpp>

// Encryptionkey for internal hashing.
uint8_t Encryptionkey[32]{};

// Authentication ticket to prove an identity.
struct Authenticationticket
{
    bool Valid;
    uint32_t ForumID;           // Needs to be cast by the module using it.
    uint32_t APIToken;          // Token combined with address to prove identity.
    uint32_t Tickettype;        // Client, Host, and any other.
    uint32_t Expiration;        // UNIX timestamp of expiration.
    uint32_t IPv4Address;       // Token combined with address to prove identity.
    std::string Username;       // The name visible on the forum.

    virtual void toStringPublic(std::string &Output) = 0;
    virtual void toStringPrivate(std::string &Output) = 0;
    virtual void toStringFullticket(std::string &Output) = 0;
    virtual void Verify(std::string *Input) = 0;
};
struct Clientticket : public Authenticationticket
{
    std::string FTPServer;      // A user provided storage location for avatars.

    virtual void toStringPublic(std::string &Output)
    {
        nlohmann::json Response;

        // Serialize the public info.
        Response["ForumID"] = ForumID;
        Response["APIToken"] = APIToken;
        Response["Username"] = Username;
        Response["FTPServer"] = FTPServer;

        Output = Response.dump(1);
    }
    virtual void toStringPrivate(std::string &Output)
    {
        nlohmann::json Response;
        Tickettype = 1;

        // Serialize the private info.
        Response["ForumID"] = ForumID;
        Response["APIToken"] = APIToken;
        Response["Tickettype"] = Tickettype;
        Response["Expiration"] = Expiration;
        Response["IPV4Address"] = IPv4Address;

        Output = Response.dump(1);
    }
    virtual void toStringFullticket(std::string &Output)
    {
        nlohmann::json Response;
        std::string Publicinfo;
        std::string Privateinfo;

        // Create public and private strings.
        toStringPrivate(Privateinfo);
        toStringPublic(Publicinfo);

        // Encrypt the private info.
        auto Encryptedprivate = std::make_unique<uint8_t[]>(Privateinfo.size());
        SM4 Encrypter(Encryptionkey);
        Encrypter.encrypt(Encryptedprivate.get(), (const uint8_t *)Privateinfo.data(), Privateinfo.size());

        // Encode the private data.
        auto Encodedstring = std::make_unique<char[]>(Privateinfo.size() * 3 + 1);
        Base64::Encode((char *)Encryptedprivate.get(), Privateinfo.size(), Encodedstring.get(), Privateinfo.size() * 3 + 1);

        // Serialize the ticket.
        Response["Public"] = Publicinfo;
        Response["Private"] = Encodedstring.get();

        Output = Response.dump(1);
    }
    virtual void Verify(std::string *Input)
    {
        // Initial request format.
        nlohmann::json Rawrequest;
        Rawrequest.parse(*Input);
        std::string Privateinfo;
        Privateinfo = Rawrequest["Private"].get<std::string>();

        // Decode the private info.
        std::string Decodedprivateinfo;
        Base64::Decode(Privateinfo, &Decodedprivateinfo);

        // Decrypt the private info.
        auto Plainprivateinfo = std::make_unique<uint8_t[]>(Decodedprivateinfo.size());
        SM4 Decrypter(Encryptionkey);
        Decrypter.decrypt(Plainprivateinfo.get(), (const uint8_t *)Decodedprivateinfo.data(), Decodedprivateinfo.size());

        // Deserialize private info.
        nlohmann::json Plainrequest;
        Plainrequest.parse((char *)Plainprivateinfo.get());
        ForumID = Plainrequest["ForumID"];
        APIToken = Plainrequest["APIToken"];
        Tickettype = Plainrequest["Tickettype"];
        Expiration = Plainrequest["Expiration"];
        IPv4Address = Plainrequest["IPv4Address"];

        // Verify the tickets timestamp.
        Valid = std::time(NULL) < Expiration;
    }
};

struct Clientauthentication : public IService
{
    virtual std::string Servicename() { return "/Clientauthentication"; };
    virtual bool Handlerequest(mg_connection *Connection, http_message *Request)
    {
        char Email[256]{};
        char Encodedpassword[255]{};

        // Get the variables.
        mg_get_http_var(&Request->query_string, "email", Email, sizeof(Email));
        mg_get_http_var(&Request->query_string, "password", Encodedpassword, sizeof(Encodedpassword));

        /*
            TODO(Convery):
            Get a copy of the forum database, verify the credentials.
        */
        return false;
    }
    Clientauthentication() 
    { 
        // Generate the encryptionkey.        
        SM3 Hasher((uint8_t *)ENCRYPTION_SEED, std::strlen(ENCRYPTION_SEED));
        Hasher.digest(Encryptionkey);

        Service::Register(this); 
    }
};
static Clientauthentication Serviceloader;
