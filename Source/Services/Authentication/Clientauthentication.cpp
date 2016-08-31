/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-8-9
    Notes:
        Users are authenticated against a local SQLite DB.
*/

#include <Configuration\All.h>
#include <Services\Servicehandler.h>
#include <unordered_map>
#include <sm4_cl.hpp>
#include <sm3_cl.hpp>
#include <json.hpp>
#include <base64.h>
#include <mutex>

// Encryptionkey that should be unique for this session.
uint8_t Encryptionkey[32];

// Encrypted ticket data for the host to verify.
struct Verificationticket
{
    uint32_t Token;             // Proof of sucessful decryption.
    uint32_t ForumID;           // Needs to be cast by the module using it.
    uint32_t Expiration;        // UNIX timestamp of expiration.
    uint32_t IPv4Address;       // To prevent MITM problems.
    std::string Username;       // The name visible on the forum.
    
    bool Decrypt(std::string *Encryptedticket)
    {
        // Decrypt the string into a buffer.
        SM4 Decrypter(Encryptionkey);
        auto Decryptedbuffer = std::make_unique<uint8_t []>(Encryptedticket->size());
        Decrypter.decrypt(Decryptedbuffer.get(), (const uint8_t *)Encryptedticket->data(), Encryptedticket->size());
        
        // Deserialize the JSON string.
        nlohmann::json JSONTicket;
        try
        {
            Token = 0;  // Invalidate.
            JSONTicket.parse((char *)Decryptedbuffer.get());
            
            Token = JSONTicket["Token"];
            ForumID = JSONTicket["ForumID"];
            Expiration = JSONTicket["Expiration"];
            IPv4Address = JSONTicket["IPv4Address"];
        }
        catch(...) {}
        
        return Token == *(uint32_t *)Encryptionkey;        
    }
    void Encrypt(std::string *Encryptedticket)
    {
        // Serialize the ticket data.
        nlohmann::json JSONTicket;
        Token = *(uint32_t *)Encryptionkey;
        
        JSONTicket["Token"] = Token;
        JSONTicket["ForumID"] = ForumID;
        JSONTicket["Expiration"] = Expiration;
        JSONTicket["IPv4Address"] = IPv4Address;
        
        // Encrypt the JSON string.
        SM4 Encrypter(Encryptionkey);
        size_t Encryptedsize = JSONTicket.dump(1).size();
        auto Encryptedbuffer = std::make_unique<uint8_t []>(Encryptedsize);
        Encrypter.encrypt(Encryptedbuffer.get(), (const uint8_t *)JSONTicket.dump(1).c_str(), Encryptedsize);
        
        Encryptedticket->append((const char *)Encryptedbuffer.get(), Encryptedsize);
    }
};

/*
    TODO(Convery):
    Move this very basic structure to it's own file
    and expend on it as needed.
*/
struct Backendclient
{
    uint32_t ForumID;
    Verificationticket Activeticket;
};

// Tasks handled by the service.
struct Clientauthentication : public IService
{
    // Clientmap.
    std::unordered_map<uint64_t /* ForumID */, std::unique_ptr<Backendclient>> Activeclients;
    
    // Service tasks.
    bool Clientheartbeat(Verificationticket *Ticket)
    {
        if(Ticket->Expiration < uint32_t(time(NULL)))
            return false;
        
        Ticket->Expiration = uint32_t(time(NULL)) + 300000;
        return true;
    }

    // Base service.
    virtual std::string Servicename() { return "/Clientauthentication"; };
    virtual std::string Handlerequest(std::string &JSONString)
    {
        return "{ None }";
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
